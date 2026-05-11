#pragma once

#include<string>
#include<unordered_map>
#include<functional>
#include<boost/beast/http.hpp>

namespace http = boost::beast::http;

enum class HttpMethod{
    GET,
    POST
};

using RouteHandler = std::function 
<
    http::response<http::string_body>(const http::request<http::string_body>&) 
>;

class Router{
    private:

        std::unordered_map<std::string, RouteHandler> getRoutes;

        std::unordered_map<std::string, RouteHandler> postRoutes;

    public:

        void RegisterRoute(
            HttpMethod method,
            const std::string& path,
            RouteHandler handler
        );

        http::response<http::string_body> HandleRequest(
            const http::request<http::string_body>& request
        );
};