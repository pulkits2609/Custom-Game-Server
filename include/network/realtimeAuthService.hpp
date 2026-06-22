#pragma once

#include "clientConnection.hpp"
#include "connectionManager.hpp"
#include "presenceService.hpp"
#include "serverEventDispatcher.hpp"
#include "../session/sessionManager.hpp"

#include <boost/json.hpp>
#include <memory>
#include <string>

class RealtimeAuthService{
private:
    ConnectionManager& connectionManager;
    SessionManager& sessionManager;
    PresenceService& presenceService;
    ServerEventDispatcher& eventDispatcher;

    static std::string ExtractToken(
        const boost::json::object& body
    );

public:
    RealtimeAuthService(
        ConnectionManager& connectionManager,
        SessionManager& sessionManager,
        PresenceService& presenceService,
        ServerEventDispatcher& eventDispatcher
    );

    bool HandleMessage(
        const std::shared_ptr<ClientConnection>& connection,
        const std::string& message
    );
};