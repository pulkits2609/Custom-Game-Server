#include "../../include/network/realtimeAuthService.hpp"

#include "../../include/network/message.hpp"

#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace json = boost::json;

RealtimeAuthService::RealtimeAuthService(
    ConnectionManager& connectionManager,
    SessionManager& sessionManager
)
    : connectionManager(connectionManager),
      sessionManager(sessionManager)
{
}

std::string RealtimeAuthService::ExtractToken(
    const boost::json::object& body
){
    auto TokenIt = body.find("token");

    if(
        TokenIt != body.end() &&
        TokenIt->value().is_string()
    ){
        return TokenIt->value().as_string().c_str();
    }

    auto DataIt = body.find("data");

    if(
        DataIt != body.end() &&
        DataIt->value().is_object()
    ){
        auto& DataObject = DataIt->value().as_object();

        auto NestedTokenIt = DataObject.find("token");

        if(
            NestedTokenIt != DataObject.end() &&
            NestedTokenIt->value().is_string()
        ){
            return NestedTokenIt->value().as_string().c_str();
        }
    }

    return "";
}

bool RealtimeAuthService::HandleMessage(
    const std::shared_ptr<ClientConnection>& connection,
    const std::string& message
){
    if(!connection || !connection->IsOpen()){
        return false;
    }

    try{
        json::value Parsed = json::parse(message);

        if(!Parsed.is_object()){
            connection->Send(
                Message::BuildEvent("AuthenticationFailed")
            );
            return true;
        }

        auto& Body = Parsed.as_object();

        auto EventIt = Body.find("event");

        if(
            EventIt == Body.end() ||
            !EventIt->value().is_string()
        ){
            return false;
        }

        std::string Event = EventIt->value().as_string().c_str();

        if(Event != "Authenticate"){
            return false;
        }

        if(connection->IsAuthenticated()){
            connection->Send(
                Message::BuildEvent("AuthenticationFailed")
            );
            return true;
        }

        std::string TokenString = ExtractToken(Body);

        if(TokenString.empty()){
            connection->Send(
                Message::BuildEvent("AuthenticationFailed")
            );
            return true;
        }

        boost::uuids::uuid Token;
        try{
            boost::uuids::string_generator Generator;
            Token = Generator(TokenString);
        }
        catch(const std::exception&){
            connection->Send(
                Message::BuildEvent("AuthenticationFailed")
            );
            return true;
        }

        auto Session = sessionManager.ValidateToken(Token);

        if(!Session){
            connection->Send(
                Message::BuildEvent("AuthenticationFailed")
            );
            return true;
        }

        if(
            !connectionManager.AuthenticateConnection(
                TokenString,
                Session->GetUsername(),
                connection
            )
        ){
            connection->Send(
                Message::BuildEvent("AuthenticationFailed")
            );
            return true;
        }

        json::object ResponseData;
        ResponseData["username"] = Session->GetUsername();
        ResponseData["playerName"] = Session->GetPlayerName();
        ResponseData["sessionToken"] = TokenString;

        connection->Send(
            Message::BuildEvent("Authenticated", ResponseData)
        );

        return true;
    }
    catch(const std::exception&){
        connection->Send(
            Message::BuildEvent("AuthenticationFailed")
        );
        return true;
    }
}