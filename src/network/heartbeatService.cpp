#include "../../include/network/heartbeatService.hpp"

#include <boost/json.hpp>

#include <iostream>
#include <vector>

namespace json = boost::json;

HeartbeatService::HeartbeatService(
    ConnectionManager& connectionManager
)
    : connectionManager(connectionManager),
      running(true),
      monitorThread(&HeartbeatService::MonitorLoop, this)
{
}

HeartbeatService::~HeartbeatService(){
    running = false;

    if(monitorThread.joinable()){
        monitorThread.join();
    }
}

void HeartbeatService::RegisterPlayer(
    const std::string& username,
    const std::shared_ptr<ClientConnection>& connection
){
    if(username.empty() || !connection){
        return;
    }

    std::lock_guard<std::mutex> Lock(mutex);
    lastPongByUsername[username] = std::chrono::steady_clock::now();
}

void HeartbeatService::UnregisterPlayer(
    const std::string& username
){
    if(username.empty()){
        return;
    }

    std::lock_guard<std::mutex> Lock(mutex);
    lastPongByUsername.erase(username);
}

void HeartbeatService::Touch(
    const std::string& username
){
    if(username.empty()){
        return;
    }

    std::lock_guard<std::mutex> Lock(mutex);

    auto It = lastPongByUsername.find(username);
    if(It == lastPongByUsername.end()){
        return;
    }

    It->second = std::chrono::steady_clock::now();
}

bool HeartbeatService::HandleMessage(
    const std::shared_ptr<ClientConnection>& connection,
    const std::string& message
){
    if(!connection){
        return false;
    }

    try{
        json::value Parsed = json::parse(message);

        if(!Parsed.is_object()){
            return false;
        }

        auto& Body = Parsed.as_object();

        auto EventIt = Body.find("event");
        if(
            EventIt == Body.end() ||
            !EventIt->value().is_string()
        ){
            return false;
        }

        std::string Event = EventIt->value().as_string().c_str();

        if(Event != "Pong"){
            return false;
        }

        const std::string Username = connection->GetUsername();

        if(Username.empty()){
            return true;
        }

        Touch(Username);
        return true;
    }
    catch(const std::exception&){
        return false;
    }
}

void HeartbeatService::MonitorLoop(){
    auto LastPingSent = std::chrono::steady_clock::now();

    while(running){
        std::this_thread::sleep_for(MonitorTick);

        const auto Now = std::chrono::steady_clock::now();

        if(Now - LastPingSent >= PingInterval){
            connectionManager.Broadcast(
                Message::BuildEvent("Ping")
            );

            LastPingSent = Now;
        }

        std::vector<std::string> TimedOutUsers;

        {
            std::lock_guard<std::mutex> Lock(mutex);

            for(const auto& Pair : lastPongByUsername){
                const auto& Username = Pair.first;
                const auto& LastPong = Pair.second;

                if(Now - LastPong >= TimeoutWindow){
                    TimedOutUsers.push_back(Username);
                }
            }
        }

        for(const auto& Username : TimedOutUsers){
            auto Connection =
                connectionManager.FetchConnectionByUsername(
                    Username
                );

            if(Connection){
                Connection->Close();
            }
            else{
                UnregisterPlayer(Username);
            }
        }
    }
}