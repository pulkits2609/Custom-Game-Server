#pragma once

#include"lobby.hpp"
#include<unordered_map>
#include<memory>
#include<boost/uuid/uuid.hpp>
#include<boost/container_hash/hash.hpp>

class LobbyManager{
    private:
        std::unordered_map<
        boost::uuids::uuid, 
        std::shared_ptr<Lobby>, 
        boost::hash<boost::uuids::uuid>
        > lobbies;

    public:
        std::shared_ptr<Lobby> CreateLobby(
            const std::string& hostId,
            const std::string& lobbyName,
            int maxPlayers
        );

        std::shared_ptr<Lobby> FetchLobby(const boost::uuids::uuid& lobbyId);
};