#include "../../include/lobby/lobbyManager.hpp"

std::shared_ptr<Lobby> LobbyManager::CreateLobby(const std::string& hostId, const std::string& lobbyName, int maxPlayers){
    auto NewLobby = std::make_shared<Lobby> (hostId, lobbyName, maxPlayers);
    NewLobby->AddMember(hostId);
    lobbies[NewLobby->GetLobbyId()] = NewLobby;
    return NewLobby;
}

std::shared_ptr<Lobby> LobbyManager::FetchLobby(const boost::uuids::uuid& lobbyId){
    auto It = lobbies.find(lobbyId);

    if(It != lobbies.end()){
        return It->second;
    }
    return nullptr;
}

bool LobbyManager::DestroyLobby(const boost::uuids::uuid& lobbyId){
    auto It = lobbies.find(lobbyId);

    if(It == lobbies.end()){
        return false;
    }

    lobbies.erase(It);
    return true;
}

std::vector<std::shared_ptr<Lobby>> LobbyManager::FetchAllLobbies() const{
    std::vector<std::shared_ptr<Lobby>> AllLobbies;

    for(const auto& Pair : lobbies){
        AllLobbies.push_back(Pair.second);
    }

    return AllLobbies;
}

std::shared_ptr<Lobby> LobbyManager::FetchLobbyByMember(
    const std::string& username
) const{
    for(const auto& Pair : lobbies){
        const auto& Lobby = Pair.second;

        if(Lobby && Lobby->IsMember(username)){
            return Lobby;
        }
    }

    return nullptr;
}