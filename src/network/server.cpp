#include "../../include/network/server.hpp"
#include <iostream>

Server::Server():acceptor(ioContext){}

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

            http::read(Socket,Buffer,Request);

            http::response<http::string_body> Response;

            std::string Target= std::string(Request.target());

            if(Request.method() == http::verb::post && Target == "/lobby/create"){
                Response= routes.CreateLobby(Request);
            }
            else if(Request.method() == http::verb::get && Target.find("/lobby/fetch/") == 0){
                Response=routes.FetchLobby(Request);
            }

            else{
                Response={
                    http::status::not_found,Request.version()
                };
                Response.set(
                    http::field::content_type,
                    "text/plain"
                );
                Response.body()="Route Not Found";
                Response.prepare_payload();
            }
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