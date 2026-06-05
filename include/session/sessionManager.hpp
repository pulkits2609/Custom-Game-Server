#pragma once

#include <memory>
#include <unordered_map>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_hash.hpp>

#include "session.hpp"

class SessionManager{
private:
    std::unordered_map<
        boost::uuids::uuid,
        std::shared_ptr<Session>,
        std::hash<boost::uuids::uuid>
    > sessionsById;

    std::unordered_map<
        boost::uuids::uuid,
        std::shared_ptr<Session>,
        std::hash<boost::uuids::uuid>
    > sessionsByToken;

public:
    std::shared_ptr<Session> CreateSession(
        const std::string& playerName,
        const std::string& username
    );

    std::shared_ptr<Session> FetchSession(
        const boost::uuids::uuid& sessionId
    );

    void DestroySession(
        const boost::uuids::uuid& sessionId
    );

    std::shared_ptr<Session> ValidateToken(
        const boost::uuids::uuid& token
    );

    void CleanupExpiredSessions();

    void DestroySessionByToken(const boost::uuids::uuid& token);
};