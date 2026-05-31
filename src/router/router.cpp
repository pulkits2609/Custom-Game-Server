#include "../../include/router/router.hpp"
#include <sstream>
#include <iostream>

std::vector<std::string> Router::SplitPath(const std::string& path){
    std::vector<std::string> segments;
    std::stringstream ss(path);
    std::string segment;

    while(std::getline(ss, segment, '/')){
        if(!segment.empty()){
            segments.push_back(segment);
        }
    }
    return segments;
}

bool Router::TryMatch(
    const RouteEntry& entry,
    HttpMethod method,
    const std::vector<std::string>& requestSegments,
    RouteParams& outParams
) const{
    if(entry.method != method) return false;
    if(entry.segments.size() != requestSegments.size()) return false;

    RouteParams params;
    for(size_t i = 0; i < entry.segments.size(); ++i){
        const std::string& pattern = entry.segments[i];
        const std::string& actual  = requestSegments[i];

        if(pattern[0] == ':'){
            params[pattern.substr(1)] = actual;
        } else if(pattern != actual){
            return false;
        }
    }

    outParams = std::move(params);
    return true;
}

void Router::RegisterRoute(
    HttpMethod method,
    const std::string& pattern,
    RouteHandler handler
){
    RouteEntry entry;
    entry.method   = method;
    entry.segments = SplitPath(pattern);
    entry.handler  = std::move(handler);
    routes.push_back(std::move(entry));
}

http::response<http::string_body> Router::HandleRequest(
    const http::request<http::string_body>& request
){
    // Strip query string before matching
    std::string target = std::string(request.target());
    auto queryPos = target.find('?');
    if(queryPos != std::string::npos){
        target = target.substr(0, queryPos);
    }

    HttpMethod method;
    if(request.method() == http::verb::get){
        method = HttpMethod::GET;
    } else if(request.method() == http::verb::post){
        method = HttpMethod::POST;
    } else{
        http::response<http::string_body> response{
            http::status::method_not_allowed,
            request.version()
        };
        response.body() = "Method Not Allowed";
        response.prepare_payload();
        return response;
    }

    auto requestSegments = SplitPath(target);
    RouteParams params;

    for(const auto& entry : routes){
        if(TryMatch(entry, method, requestSegments, params)){
            return entry.handler(request, params);
        }
    }

    http::response<http::string_body> response{
        http::status::not_found,
        request.version()
    };
    response.body() = "Not Found";
    response.prepare_payload();
    return response;
}