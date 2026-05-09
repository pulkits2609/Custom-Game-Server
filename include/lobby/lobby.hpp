#pragma once
#include<string>
#include<boost/uuid/uuid.hpp>

class Lobby{
    private:

        boost::uuids::uuid lobbyId;
        std::string hostId;
        std::string lobbyName;
        int maxPlayers;

    public:
    Lobby(const std::string& hostId,const std::string& lobbyName,int maxPlayers);

    const boost::uuids::uuid& GetLobbyId() const;
    const std::string& GetHostId() const;
    const std::string& GetLobbyName() const;
    int GetMaxPlayers() const;
};