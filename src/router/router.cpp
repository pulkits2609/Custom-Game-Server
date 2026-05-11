#include "../../include/router/router.hpp"

void Router::RegisterRoute(HttpMethod method, const std::string& path, RouteHandler handler){
    switch(method){
        case HttpMethod::GET:
            getRoutes[path] = handler;
            break;
        case HttpMethod::POST:
            postRoutes[path] = handler;
            break;
    }
}

http::response<http::string_body> Router::HandleRequest(
    const http::request<http::string_body>& request
){
    std::string Target = std::string(request.target());
    RouteHandler* Handler = nullptr;
    if(request.method() == http::verb::get){
        auto It = getRoutes.find(Target);
        if(It != getRoutes.end()){
            Handler = &It->second;
        }
    }
    else if(request.method() == http::verb::post){
        auto It = postRoutes.find(Target);
        if(It != postRoutes.end()){
            Handler = &It->second;
        }
    }
    if(Handler){
        return (*Handler)(request);
    }
    http::response<http::string_body> Response{
        http::status::not_found,
        request.version()
    };
    Response.set(
        http::field::content_type,
        "text/plain"
    );
    Response.body() = "Route Not Found";
    Response.prepare_payload();
    return Response;
}