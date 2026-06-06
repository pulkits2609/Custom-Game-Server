#include "../../include/routes/lobbyRoutes.hpp"

#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/string_generator.hpp>
#define BOOST_JSON_NO_LIB
#include <boost/json.hpp>
#include <iostream>

LobbyRoutes::LobbyRoutes(LobbyManager& manager, AuthMiddleware& authMiddleware):manager(manager), authMiddleware(authMiddleware){}

namespace json = boost::json;
http::response<http::string_body>

LobbyRoutes::CreateLobby(const http::request<http::string_body>& Request,const RouteParams& params){
    auto Session =
        authMiddleware.Authenticate(Request);

    if(!Session){

        http::response<http::string_body> Response{
            http::status::unauthorized,
            Request.version()
        };

        Response.body() = "Unauthorized";
        Response.prepare_payload();

        return Response;
    }

    json::value ParsedBody=json::parse(Request.body());

    json::object Body=ParsedBody.as_object();

    std::string HostID = Session->GetUsername();

    std::string LobbyName=Body["lobbyName"].as_string().c_str();

    int MaxPlayers= Body["maxPlayers"].as_int64();
    auto Lobby = manager.CreateLobby(HostID, LobbyName, MaxPlayers);

    json::object ResponseBody;
    ResponseBody["message"]= "Lobby Created";

    ResponseBody["lobbyID"]= boost::uuids::to_string(Lobby->GetLobbyId());

    http::response<http::string_body> Response{
        http::status::ok,
        Request.version()
    };

    Response.set(http::field::content_type, "application/json");
    Response.body()= json::serialize(ResponseBody);
    Response.prepare_payload();

    return Response;
}

http::response<http::string_body> LobbyRoutes::FetchLobby(
    const http::request<http::string_body>& Request,
    const RouteParams& params
){
    auto Session =
        authMiddleware.Authenticate(Request);

    if(!Session){

        http::response<http::string_body> Response{
            http::status::unauthorized,
            Request.version()
        };

        Response.body() = "Unauthorized";
        Response.prepare_payload();

        return Response;
    }

    auto It = params.find("LobbyID");

    if(It == params.end()){

        http::response<http::string_body> Response{
            http::status::bad_request,
            Request.version()
        };

        Response.body() = "Missing LobbyID";
        Response.prepare_payload();

        return Response;
    }

    boost::uuids::uuid LobbyID;

    try{

        boost::uuids::string_generator Generator;

        LobbyID =
            Generator(It->second);
    }
    catch(const std::exception&){

        http::response<http::string_body> Response{
            http::status::bad_request,
            Request.version()
        };

        Response.body() =
            "Invalid LobbyID Format";

        Response.prepare_payload();

        return Response;
    }

    auto Lobby =
        manager.FetchLobby(
            LobbyID
        );

    if(!Lobby){

        http::response<http::string_body> Response{
            http::status::not_found,
            Request.version()
        };

        Response.body() =
            "Lobby Not Found";

        Response.prepare_payload();

        return Response;
    }

    bool IsHost =
        Lobby->IsHost(
            Session->GetUsername()
        );

    bool IsMember =
        Lobby->IsMember(
            Session->GetUsername()
        );

    json::object ResponseBody;

    ResponseBody["LobbyID"] =
        boost::uuids::to_string(
            Lobby->GetLobbyId()
        );

    ResponseBody["LobbyName"] =
        Lobby->GetLobbyName();

    ResponseBody["CurrentPlayers"] = Lobby->GetCurrentPlayerCount();

    ResponseBody["MaxPlayers"] =
        Lobby->GetMaxPlayers();

    ResponseBody["IsFull"] =
        Lobby->IsFull();

    if(IsHost || IsMember){

        ResponseBody["HostID"] =
            Lobby->GetHostId();

        json::array MembersArray;

        for(
            const auto& Member :
            Lobby->GetMembers()
        ){
            MembersArray.push_back(
                json::value(Member)
            );
        }

        ResponseBody["Members"] =
            MembersArray;
    }

    http::response<http::string_body> Response{
        http::status::ok,
        Request.version()
    };

    Response.set(
        http::field::content_type,
        "application/json"
    );

    Response.body() =
        json::serialize(
            ResponseBody
        );

    Response.prepare_payload();

    return Response;
}