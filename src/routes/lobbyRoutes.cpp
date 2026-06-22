#include "../../include/routes/lobbyRoutes.hpp"

#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/string_generator.hpp>
#define BOOST_JSON_NO_LIB
#include <boost/json.hpp>
#include <iostream>

namespace json = boost::json;

LobbyRoutes::LobbyRoutes(
    LobbyManager& manager,
    AuthMiddleware& authMiddleware,
    ServerEventDispatcher& eventDispatcher
)
    : manager(manager),
      authMiddleware(authMiddleware),
      eventDispatcher(eventDispatcher)
{
}

http::response<http::string_body> LobbyRoutes::CreateLobby(
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

    json::value ParsedBody = json::parse(Request.body());
    json::object Body = ParsedBody.as_object();

    std::string HostID = Session->GetUsername();
    std::string LobbyName = Body["lobbyName"].as_string().c_str();
    int MaxPlayers = Body["maxPlayers"].as_int64();

    auto Lobby = manager.CreateLobby(
        HostID,
        LobbyName,
        MaxPlayers
    );

    eventDispatcher.NotifyLobbyCreated(Lobby);

    json::object ResponseBody;
    ResponseBody["message"] = "Lobby Created";
    ResponseBody["lobbyID"] = boost::uuids::to_string(Lobby->GetLobbyId());

    http::response<http::string_body> Response{
        http::status::ok,
        Request.version()
    };

    Response.set(http::field::content_type, "application/json");
    Response.body() = json::serialize(ResponseBody);
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
        LobbyID = Generator(It->second);
    }
    catch(const std::exception&){
        http::response<http::string_body> Response{
            http::status::bad_request,
            Request.version()
        };

        Response.body() = "Invalid LobbyID Format";
        Response.prepare_payload();
        return Response;
    }

    auto Lobby = manager.FetchLobby(LobbyID);
    if(!Lobby){
        http::response<http::string_body> Response{
            http::status::not_found,
            Request.version()
        };

        Response.body() = "Lobby Not Found";
        Response.prepare_payload();
        return Response;
    }

    bool IsHost = Lobby->IsHost(Session->GetUsername());
    bool IsMember = Lobby->IsMember(Session->GetUsername());

    json::object ResponseBody;
    ResponseBody["LobbyID"] = boost::uuids::to_string(Lobby->GetLobbyId());
    ResponseBody["LobbyName"] = Lobby->GetLobbyName();
    ResponseBody["CurrentPlayers"] = Lobby->GetCurrentPlayerCount();
    ResponseBody["MaxPlayers"] = Lobby->GetMaxPlayers();
    ResponseBody["IsFull"] = Lobby->IsFull();

    if(IsHost || IsMember){
        ResponseBody["HostID"] = Lobby->GetHostId();

        json::array MembersArray;
        for(const auto& Member : Lobby->GetMembers()){
            MembersArray.push_back(json::value(Member));
        }

        ResponseBody["Members"] = MembersArray;
    }

    http::response<http::string_body> Response{
        http::status::ok,
        Request.version()
    };

    Response.set(
        http::field::content_type,
        "application/json"
    );

    Response.body() = json::serialize(ResponseBody);
    Response.prepare_payload();
    return Response;
}

http::response<http::string_body> LobbyRoutes::JoinLobby(
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
        LobbyID = Generator(It->second);
    }
    catch(const std::exception&){
        http::response<http::string_body> Response{
            http::status::bad_request,
            Request.version()
        };

        Response.body() = "Invalid LobbyID Format";
        Response.prepare_payload();
        return Response;
    }

    auto Lobby = manager.FetchLobby(LobbyID);
    if(!Lobby){
        http::response<http::string_body> Response{
            http::status::not_found,
            Request.version()
        };

        Response.body() = "Lobby Not Found";
        Response.prepare_payload();
        return Response;
    }

    const std::string Username = Session->GetUsername();

    if(Lobby->IsMember(Username)){
        http::response<http::string_body> Response{
            http::status::conflict,
            Request.version()
        };

        Response.body() = "Already In Lobby";
        Response.prepare_payload();
        return Response;
    }

    if(Lobby->IsFull()){
        http::response<http::string_body> Response{
            http::status::conflict,
            Request.version()
        };

        Response.body() = "Lobby Is Full";
        Response.prepare_payload();
        return Response;
    }

    if(!Lobby->AddMember(Username)){
        http::response<http::string_body> Response{
            http::status::conflict,
            Request.version()
        };

        Response.body() = "Could Not Join Lobby";
        Response.prepare_payload();
        return Response;
    }

    eventDispatcher.NotifyLobbyJoined(
        Lobby,
        Username
    );

    json::object ResponseBody;
    ResponseBody["message"] = "Joined Lobby";
    ResponseBody["lobbyID"] = boost::uuids::to_string(Lobby->GetLobbyId());

    http::response<http::string_body> Response{
        http::status::ok,
        Request.version()
    };

    Response.set(
        http::field::content_type,
        "application/json"
    );

    Response.body() = json::serialize(ResponseBody);
    Response.prepare_payload();
    return Response;
}

http::response<http::string_body> LobbyRoutes::LeaveLobby(
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
        LobbyID = Generator(It->second);
    }
    catch(const std::exception&){
        http::response<http::string_body> Response{
            http::status::bad_request,
            Request.version()
        };

        Response.body() = "Invalid LobbyID Format";
        Response.prepare_payload();
        return Response;
    }

    auto Lobby = manager.FetchLobby(LobbyID);
    if(!Lobby){
        http::response<http::string_body> Response{
            http::status::not_found,
            Request.version()
        };

        Response.body() = "Lobby Not Found";
        Response.prepare_payload();
        return Response;
    }

    const std::string Username = Session->GetUsername();

    if(!Lobby->IsMember(Username)){
        http::response<http::string_body> Response{
            http::status::conflict,
            Request.version()
        };

        Response.body() = "Not In Lobby";
        Response.prepare_payload();
        return Response;
    }

    if(Lobby->IsHost(Username)){
        auto Members = Lobby->GetMembers();

        Lobby->ClearMembers();

        if(!manager.DestroyLobby(LobbyID)){
            http::response<http::string_body> Response{
                http::status::internal_server_error,
                Request.version()
            };

            Response.body() = "Could Not Destroy Lobby";
            Response.prepare_payload();
            return Response;
        }

        eventDispatcher.NotifyLobbyDestroyed(
            Lobby,
            Members
        );

        json::object ResponseBody;
        ResponseBody["message"] = "Lobby Destroyed";

        http::response<http::string_body> Response{
            http::status::ok,
            Request.version()
        };

        Response.set(http::field::content_type, "application/json");
        Response.body() = json::serialize(ResponseBody);
        Response.prepare_payload();
        return Response;
    }

    if(!Lobby->RemoveMember(Username)){
        http::response<http::string_body> Response{
            http::status::conflict,
            Request.version()
        };

        Response.body() = "Could Not Leave Lobby";
        Response.prepare_payload();
        return Response;
    }

    eventDispatcher.NotifyLobbyLeft(
        Lobby,
        Username
    );

    json::object ResponseBody;
    ResponseBody["message"] = "Left Lobby";
    ResponseBody["lobbyID"] = boost::uuids::to_string(Lobby->GetLobbyId());

    http::response<http::string_body> Response{
        http::status::ok,
        Request.version()
    };

    Response.set(http::field::content_type, "application/json");
    Response.body() = json::serialize(ResponseBody);
    Response.prepare_payload();
    return Response;
}

http::response<http::string_body> LobbyRoutes::ListLobbies(
    const http::request<http::string_body>& Request,
    const RouteParams& params
){
    json::array LobbiesArray;

    auto Lobbies = manager.FetchAllLobbies();

    for(const auto& Lobby : Lobbies){
        json::object LobbyObject;

        LobbyObject["LobbyID"] = boost::uuids::to_string(Lobby->GetLobbyId());
        LobbyObject["LobbyName"] = Lobby->GetLobbyName();
        LobbyObject["CurrentPlayers"] = Lobby->GetCurrentPlayerCount();
        LobbyObject["MaxPlayers"] = Lobby->GetMaxPlayers();
        LobbyObject["IsFull"] = Lobby->IsFull();

        LobbiesArray.push_back(LobbyObject);
    }

    http::response<http::string_body> Response{
        http::status::ok,
        Request.version()
    };

    Response.set(
        http::field::content_type,
        "application/json"
    );

    Response.body() = json::serialize(LobbiesArray);
    Response.prepare_payload();
    return Response;
}

http::response<http::string_body> LobbyRoutes::DestroyLobby(
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
        LobbyID = Generator(It->second);
    }
    catch(const std::exception&){
        http::response<http::string_body> Response{
            http::status::bad_request,
            Request.version()
        };

        Response.body() = "Invalid LobbyID Format";
        Response.prepare_payload();
        return Response;
    }

    auto Lobby = manager.FetchLobby(LobbyID);
    if(!Lobby){
        http::response<http::string_body> Response{
            http::status::not_found,
            Request.version()
        };

        Response.body() = "Lobby Not Found";
        Response.prepare_payload();
        return Response;
    }

    if(!Lobby->IsHost(Session->GetUsername())){
        http::response<http::string_body> Response{
            http::status::forbidden,
            Request.version()
        };

        Response.body() = "Only Host Can Destroy Lobby";
        Response.prepare_payload();
        return Response;
    }

    auto Members = Lobby->GetMembers();

    Lobby->ClearMembers();

    if(!manager.DestroyLobby(LobbyID)){
        http::response<http::string_body> Response{
            http::status::internal_server_error,
            Request.version()
        };

        Response.body() = "Could Not Destroy Lobby";
        Response.prepare_payload();
        return Response;
    }

    eventDispatcher.NotifyLobbyDestroyed(
        Lobby,
        Members
    );

    json::object ResponseBody;
    ResponseBody["message"] = "Lobby Destroyed";

    http::response<http::string_body> Response{
        http::status::ok,
        Request.version()
    };

    Response.set(http::field::content_type, "application/json");
    Response.body() = json::serialize(ResponseBody);
    Response.prepare_payload();
    return Response;
}

http::response<http::string_body> LobbyRoutes::KickPlayer(
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

    auto LobbyIt = params.find("LobbyID");
    if(LobbyIt == params.end()){
        http::response<http::string_body> Response{
            http::status::bad_request,
            Request.version()
        };

        Response.body() = "Missing LobbyID";
        Response.prepare_payload();
        return Response;
    }

    auto TargetIt = params.find("TargetUsername");
    if(TargetIt == params.end()){
        http::response<http::string_body> Response{
            http::status::bad_request,
            Request.version()
        };

        Response.body() = "Missing TargetUsername";
        Response.prepare_payload();
        return Response;
    }

    boost::uuids::uuid LobbyID;
    try{
        boost::uuids::string_generator Generator;
        LobbyID = Generator(LobbyIt->second);
    }
    catch(const std::exception&){
        http::response<http::string_body> Response{
            http::status::bad_request,
            Request.version()
        };

        Response.body() = "Invalid LobbyID Format";
        Response.prepare_payload();
        return Response;
    }

    auto Lobby = manager.FetchLobby(LobbyID);
    if(!Lobby){
        http::response<http::string_body> Response{
            http::status::not_found,
            Request.version()
        };

        Response.body() = "Lobby Not Found";
        Response.prepare_payload();
        return Response;
    }

    const std::string HostUsername = Session->GetUsername();

    if(!Lobby->IsHost(HostUsername)){
        http::response<http::string_body> Response{
            http::status::forbidden,
            Request.version()
        };

        Response.body() = "Only Host Can Kick Players";
        Response.prepare_payload();
        return Response;
    }

    const std::string TargetUsername = TargetIt->second;

    if(TargetUsername == HostUsername){
        http::response<http::string_body> Response{
            http::status::forbidden,
            Request.version()
        };

        Response.body() = "Host Cannot Kick Self";
        Response.prepare_payload();
        return Response;
    }

    if(!Lobby->IsMember(TargetUsername)){
        http::response<http::string_body> Response{
            http::status::conflict,
            Request.version()
        };

        Response.body() = "Player Not In Lobby";
        Response.prepare_payload();
        return Response;
    }

    if(!Lobby->RemoveMember(TargetUsername)){
        http::response<http::string_body> Response{
            http::status::internal_server_error,
            Request.version()
        };

        Response.body() = "Could Not Kick Player";
        Response.prepare_payload();
        return Response;
    }

    eventDispatcher.NotifyPlayerKicked(
        Lobby,
        TargetUsername
    );

    json::object ResponseBody;
    ResponseBody["message"] = "Player Kicked";
    ResponseBody["lobbyID"] = boost::uuids::to_string(Lobby->GetLobbyId());
    ResponseBody["username"] = TargetUsername;

    http::response<http::string_body> Response{
        http::status::ok,
        Request.version()
    };

    Response.set(http::field::content_type, "application/json");
    Response.body() = json::serialize(ResponseBody);
    Response.prepare_payload();
    return Response;
}