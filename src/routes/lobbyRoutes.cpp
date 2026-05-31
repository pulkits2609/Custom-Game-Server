#include "../../include/routes/lobbyRoutes.hpp"

#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/string_generator.hpp>
#define BOOST_JSON_NO_LIB
#include <boost/json.hpp>
#include <iostream>

namespace json = boost::json;
http::response<http::string_body>

LobbyRoutes::CreateLobby(const http::request<http::string_body>& Request,const RouteParams& params){
    json::value ParsedBody=json::parse(Request.body());

    json::object Body=ParsedBody.as_object();

    std::string HostID=Body["hostID"].as_string().c_str();

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

http::response<http::string_body> LobbyRoutes::FetchLobby(const http::request<http::string_body>& Request, const RouteParams& params){
    auto it = params.find("LobbyID");
    if(it == params.end()){
        http::response<http::string_body> Response{http::status::bad_request, Request.version()};
        Response.body() = "Missing LobbyID";
        Response.prepare_payload();
        return Response;
    }

        boost::uuids::uuid LobbyID;
    try{
        boost::uuids::string_generator Generator;
        LobbyID = Generator(it->second);
    } catch(const std::exception&){
        http::response<http::string_body> Response{http::status::bad_request, Request.version()};
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
        Response.body()= "Lobby Not Found";
        Response.prepare_payload();

        return Response;
    }
    json::object ResponseBody;

    ResponseBody["LobbyID"]=boost::uuids::to_string(Lobby->GetLobbyId());
    ResponseBody["HostID"]= Lobby->GetHostId();
    ResponseBody["LobbyName"]= Lobby->GetLobbyName();
    ResponseBody["MaxPlayers"]= Lobby->GetMaxPlayers();

    http::response<http::string_body> Response{
        http::status::ok,
        Request.version()
    };

    Response.set(http::field::content_type, "application/json");
    Response.body()= json::serialize(ResponseBody);
    Response.prepare_payload();

    return Response;
}