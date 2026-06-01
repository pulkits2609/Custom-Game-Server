#pragma once

#include "../router/router.hpp"
#include "../session/sessionManager.hpp"
#include <boost/beast/http.hpp>

namespace http = boost::beast::http;

class SessionRoutes{
private:
    SessionManager& manager;

public:
    explicit SessionRoutes(SessionManager& manager);

    http::response<http::string_body> Login(
        const http::request<http::string_body>& Request,
        const RouteParams& Params
    );

    http::response<http::string_body> FetchSession(
        const http::request<http::string_body>& Request,
        const RouteParams& Params
    );

    http::response<http::string_body> Logout(
        const http::request<http::string_body>& Request,
        const RouteParams& Params
    );
};