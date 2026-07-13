# Custom Game Server

A modular, scalable multiplayer backend server written in **C++17** using **Boost.Asio**, **Boost.Beast**, and **Boost.JSON**.

This project focuses on building a complete multiplayer networking architecture from scratch without relying on existing game server frameworks. It combines an HTTP API layer with a persistent realtime networking layer to provide lobby management, session authentication, event-driven synchronization, presence tracking, and automatic reconnect support.

---

# Features

## HTTP Networking

* Custom HTTP server built using **Boost.Beast**
* Modular request/response pipeline
* Dynamic route matching with URL parameter extraction
* RESTful API architecture
* JSON request and response serialization
* Clean separation between Router, Routes and Managers

---

## Session System

* UUID-based **SessionID** and **SessionToken**
* Sliding session expiration
* Login
* Fetch Session
* Logout
* Centralized session management
* Temporary in-memory credential store for testing
* Authentication reused by both HTTP and Realtime networking

---

## Lobby System

Supports the complete lobby lifecycle.

### Lobby Operations

* Create Lobby
* Fetch Lobby
* Fetch Lobby List
* Join Lobby
* Leave Lobby
* Destroy Lobby
* Kick Player

### Lobby Features

* Automatic host join during lobby creation
* Maximum player enforcement
* Member management
* Host permissions
* Role-based lobby information
* Automatic lobby destruction when host leaves
* Public lobby browser support

---

## Authentication Middleware

Centralized reusable authentication middleware.

Features include:

* Session validation
* Authorization header parsing
* Route protection
* Shared authentication logic across all protected HTTP endpoints

---

# Realtime Networking

A dedicated persistent TCP networking layer built separately from the HTTP server.

## Realtime Server

* Persistent TCP client connections
* Separate realtime listener
* Asynchronous socket communication
* Long-lived client sessions
* Event-driven architecture

---

## Client Connection

Represents one connected player.

Responsibilities include:

* Socket lifecycle management
* Async read loop
* Async write queue
* Session token binding
* Username binding
* Connection state management

---

## Connection Manager

Central registry for all connected players.

Maintains mappings for:

* ConnectionID → ClientConnection
* SessionToken → ClientConnection
* Username → ClientConnection

Provides:

* SendToClient
* SendToUsername
* Broadcast
* BroadcastToUsers

Designed to support efficient targeted event delivery.

---

## Realtime Authentication

Dedicated authentication service for socket connections.

Authentication flow:

Login (HTTP)

↓

Receive SessionToken

↓

Open Realtime Socket

↓

Authenticate using SessionToken

↓

Connection registered

↓

Player becomes available for realtime events

The existing HTTP session system is reused, eliminating duplicate authentication logic.

---

## Server Event Dispatcher

Dedicated event dispatching layer separating gameplay events from HTTP routes.

Responsibilities include:

* Notify lobby members
* Notify individual players
* Global broadcasts
* Lobby-specific broadcasts
* Presence event dispatching

This keeps route handlers focused only on business logic while the dispatcher manages realtime notifications.

---

## Presence System

Tracks the online state of every connected player.

Features:

* Online detection
* Offline detection
* Automatic reconnect window
* Reconnection handling
* Timeout detection
* Player presence events

Supports:

* PlayerConnected
* PlayerDisconnected
* PlayerReconnected
* PlayerTimedOut

---

## Heartbeat System

Server-driven heartbeat implementation.

Workflow:

Server

↓

Ping

↓

Client

↓

Pong

↓

Heartbeat updated

↓

Missing heartbeat

↓

Connection timeout

↓

Presence updated

↓

Reconnect window begins

This prevents stale or ghost connections while allowing temporary network interruptions.

---

## Event System

Realtime communication uses lightweight JSON event messages.

Examples include:

* Authenticated
* AuthenticationFailed
* LobbyCreated
* LobbyUpdated
* LobbyDestroyed
* LobbyListUpdated
* JoinedLobby
* LeftLobby
* KickedFromLobby
* PlayerConnected
* PlayerDisconnected
* PlayerReconnected
* PlayerTimedOut
* Ping
* Pong

The realtime layer is **event-driven only**.

All authoritative state changes continue to occur through HTTP APIs.

---

# Backend Architecture

The server follows a modular service-oriented architecture.

```
HTTP Layer
        │
        ▼
Session Layer
        │
        ▼
Lobby Layer
        │
        ▼
Realtime Layer
        │
        ▼
Targeted Event Distribution
```

Current services include:

* Router
* SessionManager
* LobbyManager
* AuthMiddleware
* ConnectionManager
* RealtimeServer
* RealtimeAuthService
* PresenceService
* HeartbeatService
* ServerEventDispatcher

Each component has a single responsibility, making the system modular, reusable and easy to extend.

---

# REST API Endpoints

## Session

| Method | Endpoint                    |
| ------ | --------------------------- |
| POST   | `/session/login`            |
| GET    | `/session/fetch/:SessionID` |
| POST   | `/session/logout`           |

## Lobby

| Method | Endpoint                               |
| ------ | -------------------------------------- |
| POST   | `/lobby/create`                        |
| GET    | `/lobby/list`                          |
| GET    | `/lobby/fetch/:LobbyID`                |
| POST   | `/lobby/join/:LobbyID`                 |
| POST   | `/lobby/leave/:LobbyID`                |
| POST   | `/lobby/destroy/:LobbyID`              |
| POST   | `/lobby/kick/:LobbyID/:TargetUsername` |

---

# Technologies Used

* C++17
* Boost.Asio
* Boost.Beast
* Boost.JSON
* Boost.UUID
* CMake
* Modern C++ STL
* TCP Networking
* HTTP
* JSON

---

# Project Structure

```
Custom-Game-Server/
│
├── include/
│   ├── lobby/
│   ├── middleware/
│   ├── network/
│   ├── router/
│   ├── routes/
│   └── session/
│
├── src/
│   ├── lobby/
│   ├── middleware/
│   ├── network/
│   ├── router/
│   ├── routes/
│   └── session/
│
├── CMakeLists.txt
├── README.md
└── build/
```

---

# Current Development Status

### Completed

* Custom HTTP server
* Dynamic router
* Session management
* Authentication middleware
* Complete lobby lifecycle
* Realtime TCP networking
* Socket authentication
* Event-driven architecture
* Connection management
* Presence system
* Heartbeat monitoring
* Automatic reconnect framework
* Modular service-oriented backend architecture

---

# Planned Features

* Lobby ready system
* Match start workflow
* Gameplay message dispatcher
* Player state synchronization
* Spawn system
* Realtime movement replication
* Animation synchronization
* Gameplay event replication
* Matchmaking
* Dedicated game session management

---

# Design Philosophy

This project is designed around a clear separation of responsibilities.

* **HTTP** is the authoritative layer responsible for changing server state.
* **Realtime TCP** is responsible only for notifying clients about state changes.
* **Managers** own application state.
* **Routes** coordinate business logic.
* **Services** encapsulate networking, authentication, presence, heartbeat, and event distribution.
* **Clients refresh authoritative state through HTTP after receiving realtime events**, resulting in a clean, scalable, and maintainable multiplayer architecture.
