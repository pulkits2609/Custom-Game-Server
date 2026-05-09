# Custom Game Server

A modular multiplayer backend server written in C++ using Boost.Asio and Boost.Beast.

This project is focused on building a scalable multiplayer networking architecture from scratch, including HTTP routing, lobby management, UUID-based session tracking, JSON serialization, and real-time multiplayer communication systems.

---

# Features

## HTTP Networking
- Custom HTTP server using Boost.Beast
- TCP socket accept loop architecture
- Request/response handling
- Dynamic route matching system
- Modular routing layer

## Lobby System
- UUID based lobby generation
- Lobby creation and fetching
- Centralized lobby management
- Shared pointer memory management
- Scalable multiplayer session architecture

## JSON Support
- JSON request parsing using Boost.JSON
- JSON response serialization
- Structured API communication

## Multiplayer Networking
- Hybrid TCP + UDP architecture
- Real-time multiplayer data synchronization
- Low latency packet streaming pipeline
- Foundation for authoritative server replication
- Multiplayer state maintenance system

## Backend Architecture
- Modular project structure
- Separate network/routes/lobby systems
- Clean header/source organization
- Consistent naming and formatting conventions
- Reusable server initialization pipeline

---

# Technologies Used

- C++
- Boost.Asio
- Boost.Beast
- Boost.JSON
- UUID Systems
- TCP Networking
- UDP Networking
- CMake

---

# Project Structure

```txt
Custom-Game-Server/
│
├── include/
│   ├── lobby/
│   ├── network/
│   └── routes/
│
├── src/
│   ├── lobby/
│   ├── network/
│   └── routes/
│
├── build/
├── CMakeLists.txt
└── README.md