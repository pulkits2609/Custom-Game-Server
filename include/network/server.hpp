#pragma once

#include<boost/asio.hpp>
#include<boost/beast.hpp>
#include"../router/router.hpp"
#include"../routes/lobbyRoutes.hpp"
#include "../session/sessionManager.hpp"
#include "../routes/sessionRoutes.hpp"
#include "../lobby/lobbyManager.hpp"
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
        LobbyRoutes lobbyRoutes;

        SessionManager sessionManager;
        SessionRoutes sessionRoutes;

    public:
        Server();
        bool Initialize();
        bool Bind(int port);
        bool StartListening();
        void AcceptLoop();
};