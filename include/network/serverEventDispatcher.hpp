#pragma once

#include "connectionManager.hpp"
#include "../lobby/lobby.hpp"

#include <memory>
#include <string>
#include <vector>

#include <boost/json.hpp>

class ServerEventDispatcher{
private:
    ConnectionManager& connectionManager;

    static boost::json::object BuildLobbyEventData(
        const std::shared_ptr<Lobby>& lobby,
        const std::string& username = ""
    );

    void NotifyLobbyListUpdated();

public:
    explicit ServerEventDispatcher(
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
};