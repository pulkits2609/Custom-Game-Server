#pragma once

#include <string>
#include <boost/json.hpp>

class Message{
public:
    static std::string BuildEvent(
        const std::string& event
    );

    static std::string BuildEvent(
        const std::string& event,
        const boost::json::value& data
    );
};