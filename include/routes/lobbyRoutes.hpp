#pragma once

#include "../lobby/lobbyManager.hpp"
#include "../middleware/authMiddleware.hpp"
#include "../network/connectionManager.hpp"
#include "../router/router.hpp"

#include <boost/beast/http.hpp>

namespace http = boost::beast::http;

class LobbyRoutes{
private:
    LobbyManager& manager;
    AuthMiddleware& authMiddleware;
    ConnectionManager& connectionManager;

public:
    explicit LobbyRoutes(
        LobbyManager& manager,
        AuthMiddleware& authMiddleware,
        ConnectionManager& connectionManager
    );

    http::response<http::string_body> CreateLobby(
        const http::request<http::string_body>& request,
        const RouteParams& params
    );

    http::response<http::string_body> FetchLobby(
        const http::request<http::string_body>& request,
        const RouteParams& params
    );

    http::response<http::string_body> JoinLobby(
        const http::request<http::string_body>& request,
        const RouteParams& params
    );

    http::response<http::string_body> LeaveLobby(
        const http::request<http::string_body>& request,
        const RouteParams& params
    );

    http::response<http::string_body> ListLobbies(
        const http::request<http::string_body>& request,
        const RouteParams& params
    );

    http::response<http::string_body> DestroyLobby(
        const http::request<http::string_body>& request,
        const RouteParams& params
    );

    http::response<http::string_body> KickPlayer(
        const http::request<http::string_body>& request,
        const RouteParams& params
    );
};