#include "../../include/lobby/lobby.hpp"
#include <boost/uuid/random_generator.hpp>

Lobby::Lobby(const std::string& hostId, const std::string& lobbyName, int maxPlayers):
    lobbyId(boost::uuids::random_generator()()),
    hostId(hostId),
    lobbyName(lobbyName),
    maxPlayers(maxPlayers){}

const boost::uuids::uuid& Lobby::GetLobbyId() const{
    return lobbyId;
}

const std::string& Lobby::GetHostId() const{
    return hostId;
}

const std::string& Lobby::GetLobbyName() const{
    return lobbyName;
}

int Lobby::GetMaxPlayers() const{
    return maxPlayers;
}