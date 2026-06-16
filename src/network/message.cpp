#include "../../include/network/message.hpp"

namespace json = boost::json;

std::string Message::BuildEvent(
    const std::string& event
){
    json::object Body;
    Body["event"] = event;

    std::string Payload = json::serialize(Body);
    Payload.push_back('\n');
    return Payload;
}

std::string Message::BuildEvent(
    const std::string& event,
    const boost::json::value& data
){
    json::object Body;
    Body["event"] = event;

    if(!data.is_null()){
        Body["data"] = data;
    }

    std::string Payload = json::serialize(Body);
    Payload.push_back('\n');
    return Payload;
}