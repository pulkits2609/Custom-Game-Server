#include "../../include/network/realtimeServer.hpp"

#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/json.hpp>

#include <iostream>

namespace json = boost::json;

RealtimeServer::RealtimeServer(
    ConnectionManager& connectionManager,
    SessionManager& sessionManager
)
    : acceptor(ioContext),
      connectionManager(connectionManager),
      sessionManager(sessionManager)
{
}

bool RealtimeServer::Initialize(){
    return true;
}

bool RealtimeServer::Bind(int Port){
    try{
        tcp::endpoint Endpoint(tcp::v4(), Port);

        acceptor.open(Endpoint.protocol());
        acceptor.set_option(
            asio::socket_base::reuse_address(true)
        );

        acceptor.bind(Endpoint);
        return true;
    }
    catch(const std::exception& Error){
        std::cout << "Realtime Bind Error: " << Error.what() << "\n";
        return false;
    }
}

bool RealtimeServer::StartListening(){
    try{
        acceptor.listen();
        return true;
    }
    catch(const std::exception& Error){
        std::cout << "Realtime Listen Error: " << Error.what() << "\n";
        return false;
    }
}

void RealtimeServer::HandleIncomingMessage(
    const std::shared_ptr<ClientConnection>& connection,
    const std::string& message
){
    try{
        json::value Parsed = json::parse(message);

        if(!Parsed.is_object()){
            return;
        }

        auto& Body = Parsed.as_object();

        auto EventIt = Body.find("event");
        if(EventIt == Body.end() || !EventIt->value().is_string()){
            return;
        }

        std::string Event = EventIt->value().as_string().c_str();

        if(Event != "Authenticate"){
            return;
        }

        std::string TokenString;

        auto DataIt = Body.find("data");
        if(DataIt != Body.end() && DataIt->value().is_object()){
            auto& DataObject = DataIt->value().as_object();

            auto TokenIt = DataObject.find("token");
            if(TokenIt != DataObject.end() && TokenIt->value().is_string()){
                TokenString = TokenIt->value().as_string().c_str();
            }
        }

        if(TokenString.empty()){
            auto TokenIt = Body.find("token");
            if(TokenIt != Body.end() && TokenIt->value().is_string()){
                TokenString = TokenIt->value().as_string().c_str();
            }
        }

        if(TokenString.empty()){
            connection->Send(
                Message::BuildEvent("AuthenticationFailed")
            );
            return;
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
            return;
        }

        auto Session = sessionManager.ValidateToken(Token);
        if(!Session){
            connection->Send(
                Message::BuildEvent("AuthenticationFailed")
            );
            return;
        }

        connectionManager.AuthenticateConnection(
            TokenString,
            Session->GetUsername(),
            connection
        );

        json::object ResponseData;
        ResponseData["username"] = Session->GetUsername();
        ResponseData["playerName"] = Session->GetPlayerName();

        connection->Send(
            Message::BuildEvent("Authenticated", ResponseData)
        );
    }
    catch(const std::exception& Error){
        std::cout << "Realtime Message Error: " << Error.what() << "\n";
    }
}

void RealtimeServer::AcceptLoop(){
    while(true){
        try{
            tcp::socket Socket(ioContext);
            acceptor.accept(Socket);

            auto Connection = std::make_shared<ClientConnection>(
                std::move(Socket)
            );

            connectionManager.AddConnection(Connection);

            Connection->SetMessageHandler(
                [this](const std::shared_ptr<ClientConnection>& Conn, const std::string& Message){
                    HandleIncomingMessage(Conn, Message);
                }
            );

            Connection->SetDisconnectHandler(
                [this](const std::shared_ptr<ClientConnection>& Conn){
                    connectionManager.RemoveConnection(
                        Conn->GetConnectionId()
                    );
                }
            );

            Connection->Start();
        }
        catch(const std::exception& Error){
            std::cout << "Realtime Accept Loop Error: " << Error.what() << "\n";
        }
    }
}