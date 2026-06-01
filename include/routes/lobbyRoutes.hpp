#pragma once

#include"../lobby/lobbyManager.hpp"
#include<boost/beast/http.hpp>
#include "../router/router.hpp"

namespace http = boost::beast::http;

class LobbyRoutes{
    private:
        LobbyManager& manager;

    public:
        explicit LobbyRoutes(LobbyManager& manager);

        http::response<http::string_body> CreateLobby(
            const http::request<http::string_body>& request,
            const RouteParams& params
        );

        http::response<http::string_body> FetchLobby(
            const http::request<http::string_body>& request,
            const RouteParams& params
        );
};