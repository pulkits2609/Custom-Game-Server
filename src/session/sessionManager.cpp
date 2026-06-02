#include "../../include/session/sessionManager.hpp"

std::shared_ptr<Session> SessionManager::CreateSession(
    const std::string& playerName,
    const std::string& username
){
    auto session =
        std::make_shared<Session>(
            playerName,
            username
        );

    sessionsById[session->GetSessionId()] = session;
    sessionsByToken[session->GetSessionToken()] = session;

    return session;
}

std::shared_ptr<Session> SessionManager::FetchSession(
    const boost::uuids::uuid& sessionId
){
    auto it = sessionsById.find(sessionId);

    if(it == sessionsById.end()){
        return nullptr;
    }

    return it->second;
}

void SessionManager::DestroySession(
    const boost::uuids::uuid& sessionId
){
    auto it = sessionsById.find(sessionId);

    if(it == sessionsById.end()){
        return;
    }

    sessionsByToken.erase(
        it->second->GetSessionToken()
    );

    sessionsById.erase(it);
}

bool SessionManager::ValidateToken(
    const boost::uuids::uuid& token
){

    CleanupExpiredSessions();

    auto it = sessionsByToken.find(token);

    if(it == sessionsByToken.end()){
        return false;
    }

    if(it->second->IsExpired()){
        return false;
    }

    it->second->ExtendExpiration();

    return true;
}

void SessionManager::CleanupExpiredSessions(){

    std::vector<boost::uuids::uuid> ExpiredSessions;

    for(const auto& Pair : sessionsById){

        if(Pair.second->IsExpired()){

            ExpiredSessions.push_back(
                Pair.first
            );
        }
    }

    for(const auto& SessionID : ExpiredSessions){

        DestroySession(SessionID);
    }
}

void SessionManager::DestroySessionByToken(const boost::uuids::uuid& token){
    auto it = sessionsByToken.find(token);
    if(it == sessionsByToken.end()){
        return;
    }
    sessionsById.erase(it->second->GetSessionId());
    sessionsByToken.erase(it);
}