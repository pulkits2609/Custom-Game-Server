#pragma once

#include"../lobby/lobbyManager.hpp"
#include<boost/beast/http.hpp>
#include "../router/router.hpp"
#include "../middleware/authMiddleware.hpp"

namespace http = boost::beast::http;

class LobbyRoutes{
    private:
        LobbyManager& manager;
        AuthMiddleware& authMiddleware;

    public:
        explicit LobbyRoutes(LobbyManager& manager, AuthMiddleware& authMiddleware);

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
        
};