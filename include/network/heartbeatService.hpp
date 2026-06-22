#pragma once

#include "clientConnection.hpp"
#include "connectionManager.hpp"
#include "message.hpp"

#include <atomic>
#include <chrono>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>

class HeartbeatService{
private:
    static constexpr auto PingInterval = std::chrono::seconds(20);
    static constexpr auto TimeoutWindow = std::chrono::seconds(30);
    static constexpr auto MonitorTick = std::chrono::seconds(1);

    ConnectionManager& connectionManager;

    mutable std::mutex mutex;

    std::unordered_map<
        std::string,
        std::chrono::steady_clock::time_point
    > lastPongByUsername;

    std::atomic<bool> running;
    std::thread monitorThread;

    void MonitorLoop();

public:
    explicit HeartbeatService(
        ConnectionManager& connectionManager
    );

    ~HeartbeatService();

    void RegisterPlayer(
        const std::string& username,
        const std::shared_ptr<ClientConnection>& connection
    );

    void UnregisterPlayer(
        const std::string& username
    );

    bool HandleMessage(
        const std::shared_ptr<ClientConnection>& connection,
        const std::string& message
    );

    void Touch(
        const std::string& username
    );
};