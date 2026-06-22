#include "../../include/network/presenceService.hpp"

#include <vector>
#include <iostream>

PresenceService::PresenceService(
    TimeoutHandler timeoutHandler
)
    : timeoutHandler(std::move(timeoutHandler)),
      running(true),
      monitorThread(&PresenceService::MonitorLoop, this)
{
}

PresenceService::~PresenceService(){
    running = false;

    if(monitorThread.joinable()){
        monitorThread.join();
    }
}

void PresenceService::MonitorLoop(){
    while(running){
        std::this_thread::sleep_for(std::chrono::seconds(1));

        std::vector<std::string> TimedOutUsers;

        {
            std::lock_guard<std::mutex> Lock(mutex);

            const auto Now = std::chrono::steady_clock::now();

            for(auto It = records.begin(); It != records.end(); ){
                const auto& Record = It->second;

                if(
                    !Record.IsOnline &&
                    Record.AwaitingReconnect &&
                    Now >= Record.ReconnectDeadline
                ){
                    TimedOutUsers.push_back(It->first);
                    It = records.erase(It);
                    continue;
                }

                ++It;
            }
        }

        for(const auto& Username : TimedOutUsers){
            if(timeoutHandler){
                timeoutHandler(Username);
            }
        }
    }
}

PresenceService::PresenceTransition PresenceService::MarkOnline(
    const std::string& username,
    const std::shared_ptr<ClientConnection>& connection,
    std::chrono::seconds reconnectWindow
){
    if(username.empty() || !connection){
        return PresenceTransition::AlreadyOnline;
    }

    std::lock_guard<std::mutex> Lock(mutex);

    auto& Record = records[username];

    const bool WasOnline = Record.IsOnline;
    const bool WasAwaitingReconnect = Record.AwaitingReconnect;

    Record.Connection = connection;
    Record.IsOnline = true;
    Record.AwaitingReconnect = false;
    Record.ReconnectDeadline = std::chrono::steady_clock::time_point{};

    if(WasOnline){
        return PresenceTransition::AlreadyOnline;
    }

    if(WasAwaitingReconnect){
        return PresenceTransition::Reconnected;
    }

    return PresenceTransition::NewConnection;
}

void PresenceService::MarkOffline(
    const std::string& username,
    std::chrono::seconds reconnectWindow
){
    if(username.empty()){
        return;
    }

    std::lock_guard<std::mutex> Lock(mutex);

    auto& Record = records[username];

    Record.Connection.reset();
    Record.IsOnline = false;
    Record.AwaitingReconnect = true;
    Record.ReconnectDeadline = std::chrono::steady_clock::now() + reconnectWindow;
}

bool PresenceService::IsOnline(
    const std::string& username
) const{
    std::lock_guard<std::mutex> Lock(mutex);

    auto It = records.find(username);
    if(It == records.end()){
        return false;
    }

    return It->second.IsOnline;
}

bool PresenceService::IsAwaitingReconnect(
    const std::string& username
) const{
    std::lock_guard<std::mutex> Lock(mutex);

    auto It = records.find(username);
    if(It == records.end()){
        return false;
    }

    return It->second.AwaitingReconnect;
}

void PresenceService::Remove(
    const std::string& username
){
    std::lock_guard<std::mutex> Lock(mutex);
    records.erase(username);
}