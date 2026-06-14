#include "../../include/network/server.hpp"
#include <iostream>

Server::Server():acceptor(ioContext), authMiddleware(sessionManager), lobbyRoutes(lobbyManager, authMiddleware),sessionRoutes(sessionManager){
    router.RegisterRoute(
        HttpMethod::POST,
        "/lobby/create",
        [this](const http::request<http::string_body>& req, const RouteParams& params){
            return lobbyRoutes.CreateLobby(req, params);
        }
    );

    router.RegisterRoute(
        HttpMethod::GET,
        "/lobby/fetch/:LobbyID",
        [this](const http::request<http::string_body>& req, const RouteParams& params){
            return lobbyRoutes.FetchLobby(req, params);
        }
    );

    router.RegisterRoute(
        HttpMethod::POST,
        "/session/login",
        [this](const http::request<http::string_body>& req, const RouteParams& params){
            return sessionRoutes.Login(req, params);
        }
    );

    router.RegisterRoute(
        HttpMethod::GET,
        "/session/fetch/:SessionID",
        [this](const http::request<http::string_body>& req, const RouteParams& params){
            return sessionRoutes.FetchSession(req, params);
        }
    );

    router.RegisterRoute(
        HttpMethod::POST,
        "/session/logout",
        [this](const http::request<http::string_body>& req, const RouteParams& params){
            return sessionRoutes.Logout(req, params);
        }
    );

    router.RegisterRoute(
        HttpMethod::POST,
        "/lobby/join/:LobbyID",
        [this](const http::request<http::string_body>& req, const RouteParams& params){
            return lobbyRoutes.JoinLobby(req, params);
        }
    );

    router.RegisterRoute(
        HttpMethod::POST,
        "/lobby/leave/:LobbyID",
        [this](const http::request<http::string_body>& req, const RouteParams& params){
            return lobbyRoutes.LeaveLobby(req, params);
        }
    );

    router.RegisterRoute(
        HttpMethod::GET,
        "/lobby/list",
        [this](const http::request<http::string_body>& req, const RouteParams& params){
            return lobbyRoutes.ListLobbies(req, params);
        }
    );

    router.RegisterRoute(
        HttpMethod::POST,
        "/lobby/destroy/:LobbyID",
        [this](const http::request<http::string_body>& req, const RouteParams& params){
            return lobbyRoutes.DestroyLobby(req, params);
        }
    );
}

bool Server::Initialize(){
    return true;
}

bool Server::Bind(int Port){
    try{
        tcp::endpoint Endpoint(tcp::v4(), Port);

        acceptor.open(Endpoint.protocol());
        acceptor.set_option(
            asio::socket_base::reuse_address(true)
        );

        acceptor.bind(Endpoint);
        return true;
    }

    catch(const std::exception& Error){
        std::cout<<"Bind Error: "<<Error.what()<<"\n";
        return false;
    }
}

bool Server::StartListening(){
    try{
        acceptor.listen();
        return true;
    }

    catch(const std::exception& Error){
        std::cout<<"Listen Error: "<<Error.what()<<"\n";
        return false;
    }
}

void Server::AcceptLoop(){

    while(true){
        try{
            tcp::socket Socket(ioContext);

            acceptor.accept(Socket);

            beast::flat_buffer Buffer;

            http::request<http::string_body> Request;

            http::read(Socket, Buffer, Request);

            http::response<http::string_body> Response;

            Response = router.HandleRequest(Request);

            http::write(Socket, Response);

            beast::error_code ErrorCode;

            Socket.shutdown(
                tcp::socket::shutdown_send,
                ErrorCode
            );
        }

        catch(const std::exception& Error){
            std::cout<<"Accept Loop Error: "<<Error.what()<<"\n";
        }
    }
}