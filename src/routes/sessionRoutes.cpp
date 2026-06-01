#include "../../include/routes/sessionRoutes.hpp"

#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/string_generator.hpp>
#define BOOST_JSON_NO_LIB
#include <boost/json.hpp>

namespace json = boost::json;

// Temporary in-memory credentials for testing
static const std::unordered_map<std::string, std::string> validCredentials = {
    {"Pulkit",   "password123"},
    {"TestUser", "test123"}
};

// Temporary in-memory player names for testing
static const std::unordered_map<std::string, std::string> playerNames = {
    {"Pulkit",   "Pulkit"},
    {"TestUser", "Test User"}
};

SessionRoutes::SessionRoutes(SessionManager& manager) : manager(manager){}

http::response<http::string_body> SessionRoutes::Login(
    const http::request<http::string_body>& Request,
    const RouteParams& Params
){
    json::value ParsedBody = json::parse(Request.body());
    json::object Body      = ParsedBody.as_object();

    std::string Username = Body["username"].as_string().c_str();
    std::string Password = Body["password"].as_string().c_str();

    auto credIt = validCredentials.find(Username);
    if(credIt == validCredentials.end() || credIt->second != Password){
        http::response<http::string_body> Response{http::status::unauthorized, Request.version()};
        Response.body() = "Invalid Credentials";
        Response.prepare_payload();
        return Response;
    }

    std::string PlayerName = playerNames.at(Username);
    auto Session = manager.CreateSession(PlayerName, Username);

    json::object ResponseBody;
    ResponseBody["sessionID"] = boost::uuids::to_string(Session->GetSessionId());
    ResponseBody["token"]     = boost::uuids::to_string(Session->GetSessionToken());

    http::response<http::string_body> Response{http::status::ok, Request.version()};
    Response.set(http::field::content_type, "application/json");
    Response.body() = json::serialize(ResponseBody);
    Response.prepare_payload();
    return Response;
}

http::response<http::string_body> SessionRoutes::FetchSession(
    const http::request<http::string_body>& Request,
    const RouteParams& Params
){
    auto it = Params.find("SessionID");
    if(it == Params.end()){
        http::response<http::string_body> Response{http::status::bad_request, Request.version()};
        Response.body() = "Missing SessionID";
        Response.prepare_payload();
        return Response;
    }

    boost::uuids::uuid SessionID;
    try{
        boost::uuids::string_generator Generator;
        SessionID = Generator(it->second);
    } catch(const std::exception&){
        http::response<http::string_body> Response{http::status::bad_request, Request.version()};
        Response.body() = "Invalid SessionID Format";
        Response.prepare_payload();
        return Response;
    }

    auto Session = manager.FetchSession(SessionID);
    if(!Session){
        http::response<http::string_body> Response{http::status::not_found, Request.version()};
        Response.body() = "Session Not Found";
        Response.prepare_payload();
        return Response;
    }

    json::object ResponseBody;
    ResponseBody["sessionID"]  = boost::uuids::to_string(Session->GetSessionId());
    ResponseBody["username"]   = Session->GetUsername();
    ResponseBody["playerName"] = Session->GetPlayerName();

    http::response<http::string_body> Response{http::status::ok, Request.version()};
    Response.set(http::field::content_type, "application/json");
    Response.body() = json::serialize(ResponseBody);
    Response.prepare_payload();
    return Response;
}

http::response<http::string_body> SessionRoutes::Logout(
    const http::request<http::string_body>& Request,
    const RouteParams& Params
){
    json::value ParsedBody = json::parse(Request.body());
    json::object Body      = ParsedBody.as_object();

    std::string TokenString = Body["token"].as_string().c_str();

    boost::uuids::uuid Token;
    try{
        boost::uuids::string_generator Generator;
        Token = Generator(TokenString);
    } catch(const std::exception&){
        http::response<http::string_body> Response{http::status::bad_request, Request.version()};
        Response.body() = "Invalid Token Format";
        Response.prepare_payload();
        return Response;
    }

    manager.DestroySessionByToken(Token);

    json::object ResponseBody;
    ResponseBody["message"] = "Logged Out Successfully";

    http::response<http::string_body> Response{http::status::ok, Request.version()};
    Response.set(http::field::content_type, "application/json");
    Response.body() = json::serialize(ResponseBody);
    Response.prepare_payload();
    return Response;
}