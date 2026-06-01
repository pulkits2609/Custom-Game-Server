#include "../../include/session/session.hpp"
#include <boost/uuid/random_generator.hpp>

static constexpr int SESSION_DURATION_HOURS = 1;

Session::Session(const std::string& playerName, const std::string& username):
    sessionId(boost::uuids::random_generator()()),
    sessionToken(boost::uuids::random_generator()()),
    playerName(playerName),
    username(username),
    createdAt(std::chrono::system_clock::now()),
    expiresAt(std::chrono::system_clock::now() + std::chrono::hours(SESSION_DURATION_HOURS)){}

const boost::uuids::uuid& Session::GetSessionId() const{
    return sessionId;
}

const boost::uuids::uuid& Session::GetSessionToken() const{
    return sessionToken;
}

const std::string& Session::GetPlayerName() const{
    return playerName;
}

const std::string& Session::GetUsername() const{
    return username;
}

std::chrono::system_clock::time_point Session::GetCreatedAt() const{
    return createdAt;
}

std::chrono::system_clock::time_point Session::GetExpiresAt() const{
    return expiresAt;
}

void Session::ExtendExpiration(){
    expiresAt = std::chrono::system_clock::now() + std::chrono::hours(SESSION_DURATION_HOURS);
}

bool Session::IsExpired() const{
    return std::chrono::system_clock::now() > expiresAt;
}