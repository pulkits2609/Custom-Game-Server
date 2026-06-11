#pragma once
#include<string>
#include<boost/uuid/uuid.hpp>
#include <vector>

class Lobby{
    private:
        boost::uuids::uuid lobbyId;
        std::string hostId;
        std::string lobbyName;
        int maxPlayers;
        std::vector<std::string> members;

    public:
    Lobby(const std::string& hostId,const std::string& lobbyName,int maxPlayers);

    const boost::uuids::uuid& GetLobbyId() const;
    const std::string& GetHostId() const;
    const std::string& GetLobbyName() const;
    int GetMaxPlayers() const;

    bool AddMember(
        const std::string& username
    );

    bool RemoveMember(
        const std::string& username
    );

    bool IsMember(
        const std::string& username
    ) const;

    bool IsFull() const;

    bool IsHost(
        const std::string& username
    ) const;

    const std::vector<std::string>& GetMembers() const;

    int GetCurrentPlayerCount() const;

    void ClearMembers();
};