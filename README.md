# Custom Game Server

A modular multiplayer backend server written in C++17 using Boost.Asio and Boost.Beast.

This project is focused on building a scalable multiplayer networking architecture from scratch, including HTTP routing, lobby management, session tracking, and JSON-based communication.

---

# Current Features

## HTTP Networking
- Custom HTTP server using Boost.Beast
- TCP socket accept loop
- Request/response handling
- Dynamic route matching system

## Session System
- UUID-based SessionID & SessionToken separation
- Centralized session tracking with sliding expiration
- Login, fetch session, and logout endpoints
- Temporary in-memory user credentials for testing

## Lobby System
- Lobby creation and fetching endpoints
- Automatic host join on lobby creation
- Member management: add/remove/check membership, max player enforcement
- Role-based lobby detail fetching (basic info for outsiders, full info for members)

## Middleware
- Centralized authentication via AuthMiddleware
- Reusable for all protected routes

## Backend Architecture
- Modular project structure with clean header/source separation
- Managers own state, routes reference managers
- Consistent naming and formatting
- Small, readable, and reusable classes

---

# Technologies Used

- C++17
- Boost.Asio
- Boost.Beast
- Boost.JSON
- Boost.UUID
- CMake

---

# Project Structure

```txt
Custom-Game-Server/
│
├── include/
│   ├── lobby/
│   ├── network/
│   ├── routes/
│   ├── session/
│   └── middleware/
│
├── src/
│   ├── lobby/
│   ├── network/
│   ├── routes/
│   ├── session/
│   └── middleware/
│
├── build/
├── CMakeLists.txt
└── README.md
