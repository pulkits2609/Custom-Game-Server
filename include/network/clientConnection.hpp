#pragma once

#include <memory>
#include <deque>
#include <functional>
#include <string>

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/uuid/uuid.hpp>

namespace asio = boost::asio;
namespace beast = boost::beast;

using tcp = asio::ip::tcp;

class ClientConnection
    : public std::enable_shared_from_this<ClientConnection>
{
public:
    using MessageHandler = std::function<void(
        const std::shared_ptr<ClientConnection>&,
        const std::string&
    )>;

    using DisconnectHandler = std::function<void(
        const std::shared_ptr<ClientConnection>&
    )>;

private:
    boost::uuids::uuid connectionId;
    tcp::socket socket;
    asio::strand<asio::any_io_executor> strand;
    beast::flat_buffer readBuffer;
    std::deque<std::string> writeQueue;

    bool isActive;
    std::string sessionToken;
    std::string username;

    MessageHandler messageHandler;
    DisconnectHandler disconnectHandler;

    std::string NormalizeMessage(
        const std::string& message
    ) const;

    void DoRead();
    void DoWrite();
    void NotifyDisconnect();

public:
    explicit ClientConnection(
        tcp::socket socket
    );

    const boost::uuids::uuid& GetConnectionId() const;

    tcp::socket& GetSocket();

    bool IsOpen() const;

    void Start();

    void Close();

    bool Send(
        const std::string& message
    );

    void SetMessageHandler(
        MessageHandler handler
    );

    void SetDisconnectHandler(
        DisconnectHandler handler
    );

    void SetSessionToken(
        const std::string& token
    );

    const std::string& GetSessionToken() const;

    void SetUsername(
        const std::string& username
    );

    const std::string& GetUsername() const;

    bool IsAuthenticated() const;
};