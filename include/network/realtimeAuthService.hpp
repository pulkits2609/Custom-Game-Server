#pragma once

#include "clientConnection.hpp"
#include "connectionManager.hpp"
#include "../session/sessionManager.hpp"

#include <boost/json.hpp>
#include <memory>
#include <string>

class RealtimeAuthService{
private:
    ConnectionManager& connectionManager;
    SessionManager& sessionManager;

    static std::string ExtractToken(
        const boost::json::object& body
    );

public:
    explicit RealtimeAuthService(
        ConnectionManager& connectionManager,
        SessionManager& sessionManager
    );

    bool HandleMessage(
        const std::shared_ptr<ClientConnection>& connection,
        const std::string& message
    );
};