#pragma once

#include "clientConnection.hpp"
#include "connectionManager.hpp"
#include "heartbeatService.hpp"
#include "presenceService.hpp"
#include "../session/sessionManager.hpp"

#include <boost/json.hpp>
#include <memory>
#include <string>

class ServerEventDispatcher;

class RealtimeAuthService{
private:
    ConnectionManager& connectionManager;
    SessionManager& sessionManager;
    PresenceService& presenceService;
    HeartbeatService& heartbeatService;

    static std::string ExtractToken(
        const boost::json::object& body
    );

public:
    RealtimeAuthService(
        ConnectionManager& connectionManager,
        SessionManager& sessionManager,
        PresenceService& presenceService,
        HeartbeatService& heartbeatService
    );

    bool HandleMessage(
        const std::shared_ptr<ClientConnection>& connection,
        const std::string& message
    );
};