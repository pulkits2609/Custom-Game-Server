#pragma once

#include<string>
#include<unordered_map>
#include<functional>
#include<boost/beast/http.hpp>
#include<vector>

namespace http = boost::beast::http;

enum class HttpMethod{
    GET,
    POST
};

using RouteParams =
    std::unordered_map
    <
        std::string,
        std::string
    >;

using RouteHandler = std::function
<
    http::response<http::string_body>
    (
        const http::request<http::string_body>&,
        const RouteParams&
    )
>;

class Router{
    private:

        struct RouteEntry{
            HttpMethod method;

            std::vector<std::string> segments;

            RouteHandler handler;
        };

        std::vector<RouteEntry> routes;

        static std::vector<std::string> SplitPath(
            const std::string& path
        );
        bool TryMatch(
            const RouteEntry& entry,
            HttpMethod method,
            const std::vector<std::string>& requestSegments,
            RouteParams& outParams
        ) const;

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