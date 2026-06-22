#include "../../include/network/realTimeServer.hpp"

#include <iostream>

RealtimeServer::RealtimeServer(
    ConnectionManager& connectionManager,
    SessionManager& sessionManager,
    ServerEventDispatcher& dispatcher
)
    : acceptor(ioContext),
      connectionManager(connectionManager),
      sessionManager(sessionManager),
      eventDispatcher(dispatcher),
        presenceService(
            [this](const std::string& username)
            {
                eventDispatcher.NotifyPlayerTimedOut(username);
            }
        ),
      authService(
          connectionManager,
          sessionManager,
          presenceService,
          eventDispatcher
      )
{
}

bool RealtimeServer::Initialize(){
    return true;
}

bool RealtimeServer::Bind(int Port){
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
        std::cout << "Realtime Bind Error: " << Error.what() << "\n";
        return false;
    }
}

bool RealtimeServer::StartListening(){
    try{
        acceptor.listen();
        return true;
    }
    catch(const std::exception& Error){
        std::cout << "Realtime Listen Error: " << Error.what() << "\n";
        return false;
    }
}

void RealtimeServer::HandleIncomingMessage(
    const std::shared_ptr<ClientConnection>& connection,
    const std::string& message
){
    authService.HandleMessage(connection, message);
}

void RealtimeServer::AcceptLoop(){
    while(true){
        try{
            tcp::socket Socket(ioContext);
            acceptor.accept(Socket);

            auto Connection = std::make_shared<ClientConnection>(
                std::move(Socket)
            );

            connectionManager.AddConnection(Connection);

            Connection->SetMessageHandler(
                [this](
                    const std::shared_ptr<ClientConnection>& Conn,
                    const std::string& Message
                ){
                    HandleIncomingMessage(Conn, Message);
                }
            );

            Connection->SetDisconnectHandler(
                [this](
                    const std::shared_ptr<ClientConnection>& Conn
                ){
                    const std::string Username = Conn->GetUsername();

                    if(!Username.empty()){
                        presenceService.MarkOffline(Username);
                        eventDispatcher.NotifyPlayerDisconnected(Username);
                    }

                    connectionManager.RemoveConnection(
                        Conn->GetConnectionId()
                    );
                }
            );

            Connection->Start();
        }
        catch(const std::exception& Error){
            std::cout << "Realtime Accept Loop Error: " << Error.what() << "\n";
        }
    }
}