#pragma once

#include "clientConnection.hpp"
#include "connectionManager.hpp"
#include "realtimeAuthService.hpp"
#include "presenceService.hpp"
#include "serverEventDispatcher.hpp"
#include "../session/sessionManager.hpp"

#include <boost/asio.hpp>
#include <boost/beast.hpp>

namespace asio = boost::asio;
namespace beast = boost::beast;

using tcp = asio::ip::tcp;

class RealtimeServer{
private:
    asio::io_context ioContext;
    tcp::acceptor acceptor;

    ConnectionManager& connectionManager;
    SessionManager& sessionManager;
    ServerEventDispatcher& eventDispatcher;

    PresenceService presenceService;
    RealtimeAuthService authService;

    void HandleIncomingMessage(
        const std::shared_ptr<ClientConnection>& connection,
        const std::string& message
    );

public:
    RealtimeServer(
        ConnectionManager& connectionManager,
        SessionManager& sessionManager,
        ServerEventDispatcher& eventDispatcher
    );

    bool Initialize();
    bool Bind(int port);
    bool StartListening();
    void AcceptLoop();
};