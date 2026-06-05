// what auth middleware does
// Read Authorization Header
// ↓
// Extract Bearer Token
// ↓
// Validate Token
// ↓
// Fetch Session
// ↓
// Return Session

#pragma once

#include "../session/sessionManager.hpp"

#include <memory>
#include <boost/beast/http.hpp>

namespace http = boost::beast::http;

class AuthMiddleware{
private:
    SessionManager& sessionManager;

public:
    explicit AuthMiddleware(
        SessionManager& sessionManager
    );

    std::shared_ptr<Session> Authenticate(
        const http::request<http::string_body>& request
    );
};