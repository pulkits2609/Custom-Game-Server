#include <iostream>

#include "../include/network/server.hpp"

int main(int argc, char* argv[]){

    if (argc != 2){
        std::cerr<<"Usage: ./app <PORT>\n";
        return -1;
    }

    try{
        int Port=std::stoi(argv[1]);

        Server Srv;
        if(!Srv.Initialize()){
            return -1;
        }

        if(!Srv.Bind(Port)){
            return -1;
        }
        if(!Srv.StartListening()){
            return -1;
        }
        std::cout<<"HTTP Server Started On Port "<<Port<<"\n";

        Srv.AcceptLoop();
    }
    catch(const std::exception& Error){
        std::cout<<"Server Error: "<<Error.what()<<"\n";
    }
    return 0;
}