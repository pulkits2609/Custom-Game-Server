#include "../../include/lobby/lobbyManager.hpp"

std::shared_ptr<Lobby> LobbyManager::CreateLobby(const std::string& hostId, const std::string& lobbyName, int maxPlayers){
    auto NewLobby = std::make_shared<Lobby> (hostId, lobbyName, maxPlayers);
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