#include "../../include/lobby/lobby.hpp"
#include <boost/uuid/random_generator.hpp>
#include<algorithm>

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

bool Lobby::AddMember(const std::string& username){
    if(IsFull()){
        return false;
    }
    if(IsMember(username)){
        return false;
    }
    members.push_back(username);
    return true;
}

bool Lobby::RemoveMember(
    const std::string& username
){
    auto It = std::find(
        members.begin(),
        members.end(),
        username
    );

    if(It == members.end()){
        return false;
    }

    members.erase(It);
    return true;
}

bool Lobby::IsMember(
    const std::string& username
) const{
    return std::find(
        members.begin(),
        members.end(),
        username
    ) != members.end();
}

bool Lobby::IsFull() const{
    return members.size() >= maxPlayers;
}

bool Lobby::IsHost(
    const std::string& username
) const{
    return hostId == username;
}

const std::vector<std::string>& Lobby::GetMembers() const{
    return members;
}

int Lobby::GetCurrentPlayerCount() const{
    return static_cast<int>(members.size());
}