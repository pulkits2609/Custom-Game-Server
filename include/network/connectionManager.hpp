#pragma once

#include "clientConnection.hpp"

#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

#include <boost/container_hash/hash.hpp>
#include <boost/uuid/uuid.hpp>

class ConnectionManager{
private:
    mutable std::mutex mutex;

    std::unordered_map<
        boost::uuids::uuid,
        std::shared_ptr<ClientConnection>,
        boost::hash<boost::uuids::uuid>
    > connections;

    std::unordered_map<
        std::string,
        std::shared_ptr<ClientConnection>
    > authenticatedConnectionsByToken;

    std::unordered_map<
        std::string,
        std::shared_ptr<ClientConnection>
    > authenticatedConnectionsByUsername;

public:
    void AddConnection(
        const std::shared_ptr<ClientConnection>& connection
    );

    std::shared_ptr<ClientConnection> FetchConnection(
        const boost::uuids::uuid& connectionId
    );

    bool RemoveConnection(
        const boost::uuids::uuid& connectionId
    );

    std::vector<std::shared_ptr<ClientConnection>> FetchAllConnections() const;

    std::size_t GetConnectionCount() const;
    std::size_t GetAuthenticatedConnectionCount() const;

    bool AuthenticateConnection(
        const std::string& token,
        const std::string& username,
        const std::shared_ptr<ClientConnection>& connection
    );

    std::shared_ptr<ClientConnection> FetchAuthenticatedConnection(
        const std::string& token
    );

    std::shared_ptr<ClientConnection> FetchConnectionByUsername(
        const std::string& username
    );

    bool SendToClient(
        const std::string& token,
        const std::string& message
    );

    bool SendToUsername(
        const std::string& username,
        const std::string& message
    );

    void Broadcast(
        const std::string& message
    );

    void BroadcastToUsers(
        const std::vector<std::string>& usernames,
        const std::string& message
    );
};