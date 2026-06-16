#include "../../include/network/connectionManager.hpp"

void ConnectionManager::AddConnection(
    const std::shared_ptr<ClientConnection>& connection
){
    if(!connection){
        return;
    }

    std::lock_guard<std::mutex> Lock(mutex);
    connections[connection->GetConnectionId()] = connection;
}

std::shared_ptr<ClientConnection> ConnectionManager::FetchConnection(
    const boost::uuids::uuid& connectionId
){
    std::lock_guard<std::mutex> Lock(mutex);

    auto It = connections.find(connectionId);

    if(It == connections.end()){
        return nullptr;
    }

    return It->second;
}

bool ConnectionManager::RemoveConnection(
    const boost::uuids::uuid& connectionId
){
    std::shared_ptr<ClientConnection> ConnectionToClose;

    {
        std::lock_guard<std::mutex> Lock(mutex);

        auto It = connections.find(connectionId);

        if(It == connections.end()){
            return false;
        }

        ConnectionToClose = It->second;

        if(ConnectionToClose){
            const std::string Token = ConnectionToClose->GetSessionToken();
            const std::string Username = ConnectionToClose->GetUsername();

            if(!Token.empty()){
                authenticatedConnectionsByToken.erase(Token);
            }

            if(!Username.empty()){
                authenticatedConnectionsByUsername.erase(Username);
            }
        }

        connections.erase(It);
    }

    if(ConnectionToClose){
        ConnectionToClose->Close();
    }

    return true;
}

std::vector<std::shared_ptr<ClientConnection>> ConnectionManager::FetchAllConnections() const{
    std::vector<std::shared_ptr<ClientConnection>> AllConnections;

    std::lock_guard<std::mutex> Lock(mutex);

    for(const auto& Pair : connections){
        AllConnections.push_back(Pair.second);
    }

    return AllConnections;
}

std::size_t ConnectionManager::GetConnectionCount() const{
    std::lock_guard<std::mutex> Lock(mutex);
    return connections.size();
}

std::size_t ConnectionManager::GetAuthenticatedConnectionCount() const{
    std::lock_guard<std::mutex> Lock(mutex);
    return authenticatedConnectionsByUsername.size();
}

bool ConnectionManager::AuthenticateConnection(
    const std::string& token,
    const std::string& username,
    const std::shared_ptr<ClientConnection>& connection
){
    if(!connection){
        return false;
    }

    std::lock_guard<std::mutex> Lock(mutex);

    connection->SetSessionToken(token);
    connection->SetUsername(username);

    authenticatedConnectionsByToken[token] = connection;
    authenticatedConnectionsByUsername[username] = connection;

    return true;
}

std::shared_ptr<ClientConnection> ConnectionManager::FetchAuthenticatedConnection(
    const std::string& token
){
    std::lock_guard<std::mutex> Lock(mutex);

    auto It = authenticatedConnectionsByToken.find(token);

    if(It == authenticatedConnectionsByToken.end()){
        return nullptr;
    }

    return It->second;
}

std::shared_ptr<ClientConnection> ConnectionManager::FetchConnectionByUsername(
    const std::string& username
){
    std::lock_guard<std::mutex> Lock(mutex);

    auto It = authenticatedConnectionsByUsername.find(username);

    if(It == authenticatedConnectionsByUsername.end()){
        return nullptr;
    }

    return It->second;
}

bool ConnectionManager::SendToClient(
    const std::string& token,
    const std::string& message
){
    auto Connection =
        FetchAuthenticatedConnection(
            token
        );

    if(!Connection){
        return false;
    }

    return Connection->Send(message);
}

bool ConnectionManager::SendToUsername(
    const std::string& username,
    const std::string& message
){
    auto Connection =
        FetchConnectionByUsername(
            username
        );

    if(!Connection){
        return false;
    }

    return Connection->Send(message);
}

void ConnectionManager::Broadcast(
    const std::string& message
){
    std::vector<std::shared_ptr<ClientConnection>> Snapshot;

    {
        std::lock_guard<std::mutex> Lock(mutex);

        for(const auto& Pair : authenticatedConnectionsByUsername){
            if(Pair.second){
                Snapshot.push_back(Pair.second);
            }
        }
    }

    for(const auto& Connection : Snapshot){
        Connection->Send(message);
    }
}

void ConnectionManager::BroadcastToUsers(
    const std::vector<std::string>& usernames,
    const std::string& message
){
    for(const auto& Username : usernames){
        SendToUsername(Username, message);
    }
}