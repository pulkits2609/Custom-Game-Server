#pragma once

#include "connectionManager.hpp"
#include "../lobby/lobbyManager.hpp"

#include <memory>
#include <string>
#include <vector>

#include <boost/json.hpp>

class ServerEventDispatcher{
private:
    LobbyManager& lobbyManager;
    ConnectionManager& connectionManager;

    static boost::json::object BuildLobbyEventData(
        const std::shared_ptr<Lobby>& lobby,
        const std::string& username = ""
    );

    static boost::json::object BuildPresenceEventData(
        const std::shared_ptr<Lobby>& lobby,
        const std::string& username,
        const std::string& state
    );

    std::shared_ptr<Lobby> FetchLobbyForUser(
        const std::string& username
    ) const;

    void NotifyLobbyListUpdated();

    void DispatchPresenceEvent(
        const std::string& eventName,
        const std::string& username,
        const std::string& state
    );

public:
    explicit ServerEventDispatcher(
        LobbyManager& lobbyManager,
        ConnectionManager& connectionManager
    );

    void NotifyLobbyCreated(
        const std::shared_ptr<Lobby>& lobby
    );

    void NotifyLobbyJoined(
        const std::shared_ptr<Lobby>& lobby,
        const std::string& username
    );

    void NotifyLobbyLeft(
        const std::shared_ptr<Lobby>& lobby,
        const std::string& username
    );

    void NotifyLobbyDestroyed(
        const std::shared_ptr<Lobby>& lobby,
        const std::vector<std::string>& recipients
    );

    void NotifyPlayerKicked(
        const std::shared_ptr<Lobby>& lobby,
        const std::string& username
    );

    void NotifyPlayerConnected(
        const std::string& username
    );

    void NotifyPlayerDisconnected(
        const std::string& username
    );

    void NotifyPlayerReconnected(
        const std::string& username
    );

    void NotifyPlayerTimedOut(
        const std::string& username
    );
};