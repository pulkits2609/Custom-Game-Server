#pragma once

#include "clientConnection.hpp"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>

class PresenceService{
public:
    enum class PresenceTransition{
        NewConnection,
        Reconnected,
        AlreadyOnline
    };

    using TimeoutHandler = std::function<void(const std::string&)>;

private:
    struct PresenceRecord{
        std::weak_ptr<ClientConnection> Connection;
        bool IsOnline = false;
        bool AwaitingReconnect = false;
        std::chrono::steady_clock::time_point ReconnectDeadline;
    };

    mutable std::mutex mutex;
    std::unordered_map<std::string, PresenceRecord> records;

    TimeoutHandler timeoutHandler;
    std::atomic<bool> running;
    std::thread monitorThread;

    void MonitorLoop();

public:
    explicit PresenceService(
        TimeoutHandler timeoutHandler
    );

    ~PresenceService();

    PresenceTransition MarkOnline(
        const std::string& username,
        const std::shared_ptr<ClientConnection>& connection,
        std::chrono::seconds reconnectWindow = std::chrono::seconds(30)
    );

    void MarkOffline(
        const std::string& username,
        std::chrono::seconds reconnectWindow = std::chrono::seconds(30)
    );

    bool IsOnline(
        const std::string& username
    ) const;

    bool IsAwaitingReconnect(
        const std::string& username
    ) const;

    void Remove(
        const std::string& username
    );
};