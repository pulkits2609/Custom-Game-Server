#include "../../include/middleware/authMiddleware.hpp"

#include <boost/uuid/string_generator.hpp>

AuthMiddleware::AuthMiddleware(
    SessionManager& sessionManager
)
    : sessionManager(sessionManager)
{
}

std::shared_ptr<Session> AuthMiddleware::Authenticate(
    const http::request<http::string_body>& request
){
    auto AuthHeader =
        request.find(http::field::authorization);

    if(AuthHeader == request.end()){
        return nullptr;
    }

    std::string HeaderValue = std::string(AuthHeader->value());

    const std::string Prefix = "Bearer ";

    if(HeaderValue.substr(0, Prefix.size()) != Prefix){
        return nullptr;
    }

    std::string TokenString =
        HeaderValue.substr(Prefix.size());

    try{
        boost::uuids::string_generator Generator;

        auto Token =
            Generator(TokenString);

        return sessionManager.ValidateToken(Token);
    }
    catch(...){
        return nullptr;
    }
}