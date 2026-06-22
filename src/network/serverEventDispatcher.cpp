#include "../../include/network/serverEventDispatcher.hpp"
#include "../../include/network/message.hpp"

#include <boost/uuid/uuid_io.hpp>

namespace json = boost::json;

ServerEventDispatcher::ServerEventDispatcher(
    LobbyManager& lobbyManager,
    ConnectionManager& connectionManager
)
    : lobbyManager(lobbyManager),
      connectionManager(connectionManager)
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

json::object ServerEventDispatcher::BuildPresenceEventData(
    const std::shared_ptr<Lobby>& lobby,
    const std::string& username,
    const std::string& state
){
    json::object Data;

    if(lobby){
        Data["lobbyID"] = boost::uuids::to_string(lobby->GetLobbyId());
        Data["lobbyName"] = lobby->GetLobbyName();
    }

    Data["username"] = username;
    Data["state"] = state;

    return Data;
}

std::shared_ptr<Lobby> ServerEventDispatcher::FetchLobbyForUser(
    const std::string& username
) const{
    return lobbyManager.FetchLobbyByMember(username);
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
            BuildPresenceEventData(lobby, username, "Kicked")
        )
    );

    connectionManager.BroadcastToUsers(
        lobby->GetMembers(),
        Message::BuildEvent(
            "LobbyUpdated",
            BuildPresenceEventData(lobby, username, "Kicked")
        )
    );

    NotifyLobbyListUpdated();
}

void ServerEventDispatcher::DispatchPresenceEvent(
    const std::string& eventName,
    const std::string& username,
    const std::string& state
){
    auto Lobby = FetchLobbyForUser(username);

    if(!Lobby){
        return;
    }

    connectionManager.BroadcastToUsers(
        Lobby->GetMembers(),
        Message::BuildEvent(
            eventName,
            BuildPresenceEventData(Lobby, username, state)
        )
    );
}

void ServerEventDispatcher::NotifyPlayerConnected(
    const std::string& username
){
    DispatchPresenceEvent(
        "PlayerConnected",
        username,
        "Connected"
    );
}

void ServerEventDispatcher::NotifyPlayerDisconnected(
    const std::string& username
){
    DispatchPresenceEvent(
        "PlayerDisconnected",
        username,
        "Disconnected"
    );
}

void ServerEventDispatcher::NotifyPlayerReconnected(
    const std::string& username
){
    DispatchPresenceEvent(
        "PlayerReconnected",
        username,
        "Reconnected"
    );
}

void ServerEventDispatcher::NotifyPlayerTimedOut(
    const std::string& username
){
    auto Lobby = FetchLobbyForUser(username);

    if(!Lobby){
        return;
    }

    json::object Payload = BuildPresenceEventData(
        Lobby,
        username,
        "TimedOut"
    );

    auto Members = Lobby->GetMembers();

    if(Lobby->IsHost(username)){
        connectionManager.BroadcastToUsers(
            Members,
            Message::BuildEvent(
                "PlayerTimedOut",
                Payload
            )
        );

        Lobby->ClearMembers();

        if(lobbyManager.DestroyLobby(Lobby->GetLobbyId())){
            connectionManager.BroadcastToUsers(
                Members,
                Message::BuildEvent(
                    "LobbyDestroyed",
                    BuildLobbyEventData(Lobby)
                )
            );

            NotifyLobbyListUpdated();
        }

        return;
    }

    if(Lobby->RemoveMember(username)){
        connectionManager.BroadcastToUsers(
            Lobby->GetMembers(),
            Message::BuildEvent(
                "PlayerTimedOut",
                Payload
            )
        );

        connectionManager.BroadcastToUsers(
            Lobby->GetMembers(),
            Message::BuildEvent(
                "LobbyUpdated",
                BuildLobbyEventData(Lobby, username)
            )
        );

        NotifyLobbyListUpdated();
    }
}