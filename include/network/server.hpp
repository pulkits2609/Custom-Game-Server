#pragma once

#include <boost/asio.hpp>
#include <boost/beast.hpp>

#include "../router/router.hpp"
#include "../routes/lobbyRoutes.hpp"
#include "../routes/sessionRoutes.hpp"
#include "../session/sessionManager.hpp"
#include "../lobby/lobbyManager.hpp"
#include "../middleware/authMiddleware.hpp"
#include "connectionManager.hpp"
#include "serverEventDispatcher.hpp"

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;

using tcp = asio::ip::tcp;

class Server{
private:
    asio::io_context ioContext;
    tcp::acceptor acceptor;

    Router router;

    LobbyManager lobbyManager;
    SessionManager sessionManager;
    ConnectionManager connectionManager;
    ServerEventDispatcher eventDispatcher;

    AuthMiddleware authMiddleware;

    LobbyRoutes lobbyRoutes;
    SessionRoutes sessionRoutes;

public:
    Server();

    bool Initialize();
    bool Bind(int port);
    bool StartListening();
    void AcceptLoop();

    ConnectionManager& GetConnectionManager();
    SessionManager& GetSessionManager();
    ServerEventDispatcher& GetEventDispatcher();
};