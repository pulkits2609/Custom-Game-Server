#pragma once

#include <string>
#include <chrono>
#include <boost/uuid/uuid.hpp>

class Session{
private:
    boost::uuids::uuid sessionId;
    boost::uuids::uuid sessionToken;
    std::string playerName;
    std::string username;
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point expiresAt;

public:
    Session(const std::string& playerName, const std::string& username);

    const boost::uuids::uuid& GetSessionId() const;
    const boost::uuids::uuid& GetSessionToken() const;
    const std::string& GetPlayerName() const;
    const std::string& GetUsername() const;
    std::chrono::system_clock::time_point GetCreatedAt() const;
    std::chrono::system_clock::time_point GetExpiresAt() const;

    void ExtendExpiration();
    bool IsExpired() const;
};