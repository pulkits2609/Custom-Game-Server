#pragma once

#include"../lobby/lobbyManager.hpp"
#include<boost/beast/http.hpp>
namespace http = boost::beast::http;

class LobbyRoutes{
    private:
        LobbyManager manager;

    public:
        http::response<http::string_body> CreateLobby(const http::request<http::string_body>& request);
        http::response<http::string_body> FetchLobby(const http::request<http::string_body>& request);
};