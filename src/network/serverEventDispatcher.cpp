#include "../../include/network/serverEventDispatcher.hpp"
#include "../../include/network/message.hpp"

#include <boost/uuid/uuid_io.hpp>

namespace json = boost::json;

ServerEventDispatcher::ServerEventDispatcher(
    ConnectionManager& connectionManager
)
    : connectionManager(connectionManager)
{
}

json::object ServerEventDispatcher::BuildLobbyEventData(
    const std::shared_ptr<Lobby>& lobby,
    const std::string& username
){
    json::object Data;

    if(lobby){
        Data["lobbyID"] = boost::uuids::to_string(lobby->GetLobbyId());
        Data["lobbyName"] = lobby->GetLobbyName();
    }

    if(!username.empty()){
        Data["username"] = username;
    }

    return Data;
}

void ServerEventDispatcher::NotifyLobbyListUpdated(){
    connectionManager.Broadcast(
        Message::BuildEvent("LobbyListUpdated")
    );
}

void ServerEventDispatcher::NotifyLobbyCreated(
    const std::shared_ptr<Lobby>& lobby
){
    if(!lobby){
        return;
    }

    connectionManager.SendToUsername(
        lobby->GetHostId(),
        Message::BuildEvent(
            "LobbyCreated",
            BuildLobbyEventData(lobby)
        )
    );

    NotifyLobbyListUpdated();
}

void ServerEventDispatcher::NotifyLobbyJoined(
    const std::shared_ptr<Lobby>& lobby,
    const std::string& username
){
    if(!lobby || username.empty()){
        return;
    }

    connectionManager.SendToUsername(
        username,
        Message::BuildEvent(
            "JoinedLobby",
            BuildLobbyEventData(lobby, username)
        )
    );

    connectionManager.BroadcastToUsers(
        lobby->GetMembers(),
        Message::BuildEvent(
            "LobbyUpdated",
            BuildLobbyEventData(lobby, username)
        )
    );

    NotifyLobbyListUpdated();
}

void ServerEventDispatcher::NotifyLobbyLeft(
    const std::shared_ptr<Lobby>& lobby,
    const std::string& username
){
    if(!lobby || username.empty()){
        return;
    }

    connectionManager.SendToUsername(
        username,
        Message::BuildEvent(
            "LeftLobby",
            BuildLobbyEventData(lobby, username)
        )
    );

    connectionManager.BroadcastToUsers(
        lobby->GetMembers(),
        Message::BuildEvent(
            "LobbyUpdated",
            BuildLobbyEventData(lobby, username)
        )
    );

    NotifyLobbyListUpdated();
}

void ServerEventDispatcher::NotifyLobbyDestroyed(
    const std::shared_ptr<Lobby>& lobby,
    const std::vector<std::string>& recipients
){
    if(!lobby){
        return;
    }

    connectionManager.BroadcastToUsers(
        recipients,
        Message::BuildEvent(
            "LobbyDestroyed",
            BuildLobbyEventData(lobby)
        )
    );

    NotifyLobbyListUpdated();
}

void ServerEventDispatcher::NotifyPlayerKicked(
    const std::shared_ptr<Lobby>& lobby,
    const std::string& username
){
    if(!lobby || username.empty()){
        return;
    }

    connectionManager.SendToUsername(
        username,
        Message::BuildEvent(
            "KickedFromLobby",
            BuildLobbyEventData(lobby, username)
        )
    );

    connectionManager.BroadcastToUsers(
        lobby->GetMembers(),
        Message::BuildEvent(
            "LobbyUpdated",
            BuildLobbyEventData(lobby, username)
        )
    );

    NotifyLobbyListUpdated();
}