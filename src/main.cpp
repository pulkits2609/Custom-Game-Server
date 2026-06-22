#include <iostream>
#include <thread>

#include "../include/network/server.hpp"
#include "../include/network/realtimeServer.hpp"

int main(int argc, char* argv[]){

    if(argc != 2 && argc != 3){
        std::cerr << "Usage: ./app <HTTP_PORT> [REALTIME_PORT]\n";
        return -1;
    }

    try{
        int HttpPort = std::stoi(argv[1]);
        int RealtimePort = (argc == 3)
            ? std::stoi(argv[2])
            : (HttpPort + 1);

        Server HttpServer;
        if(!HttpServer.Initialize()){
            return -1;
        }

        if(!HttpServer.Bind(HttpPort)){
            return -1;
        }

        if(!HttpServer.StartListening()){
            return -1;
        }

        RealtimeServer StreamServer(
            HttpServer.GetConnectionManager(),
            HttpServer.GetSessionManager(),
            HttpServer.GetEventDispatcher()
        );

        if(!StreamServer.Initialize()){
            return -1;
        }

        if(!StreamServer.Bind(RealtimePort)){
            return -1;
        }

        if(!StreamServer.StartListening()){
            return -1;
        }

        std::thread RealtimeThread(
            [&StreamServer](){
                StreamServer.AcceptLoop();
            }
        );

        RealtimeThread.detach();

        std::cout << "HTTP Server Started On Port " << HttpPort << "\n";
        std::cout << "Realtime Server Started On Port " << RealtimePort << "\n";

        HttpServer.AcceptLoop();
    }
    catch(const std::exception& Error){
        std::cout << "Server Error: " << Error.what() << "\n";
    }

    return 0;
}