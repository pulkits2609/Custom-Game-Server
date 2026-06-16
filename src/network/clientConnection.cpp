#include "../../include/network/clientConnection.hpp"

#include <boost/uuid/random_generator.hpp>
#include <boost/asio/buffers_iterator.hpp>
#include <istream>

ClientConnection::ClientConnection(
    tcp::socket socket
)
    : connectionId(boost::uuids::random_generator()()),
      socket(std::move(socket)),
      strand(this->socket.get_executor()),
      isActive(true)
{
}

const boost::uuids::uuid& ClientConnection::GetConnectionId() const{
    return connectionId;
}

tcp::socket& ClientConnection::GetSocket(){
    return socket;
}

bool ClientConnection::IsOpen() const{
    return socket.is_open() && isActive;
}

std::string ClientConnection::NormalizeMessage(
    const std::string& message
) const{
    if(message.empty()){
        return "\n";
    }

    if(message.back() == '\n'){
        return message;
    }

    return message + "\n";
}

void ClientConnection::Start(){
    if(!IsOpen()){
        return;
    }

    auto Self = shared_from_this();

    asio::dispatch(
        strand,
        [Self](){
            Self->DoRead();
        }
    );
}

void ClientConnection::DoRead(){
    auto Self = shared_from_this();

    asio::async_read_until(
        socket,
        readBuffer,
        '\n',
        asio::bind_executor(
            strand,
            [Self](
                const beast::error_code& Error,
                std::size_t BytesTransferred
            ){
                if(Error){
                    Self->Close();
                    Self->NotifyDisconnect();
                    return;
                }

                auto Data =
                    Self->readBuffer.data();

                std::string MessageLine(
                    boost::asio::buffers_begin(Data),
                    boost::asio::buffers_begin(Data) +
                    BytesTransferred
                );

                Self->readBuffer.consume(
                    BytesTransferred
                );

                while(
                    !MessageLine.empty() &&
                    (
                        MessageLine.back() == '\n' ||
                        MessageLine.back() == '\r'
                    )
                ){
                    MessageLine.pop_back();
                }

                if(
                    !MessageLine.empty() &&
                    Self->messageHandler
                ){
                    Self->messageHandler(
                        Self,
                        MessageLine
                    );
                }

                if(Self->IsOpen()){
                    Self->DoRead();
                }
            }
        )
    );
}

void ClientConnection::DoWrite(){
    if(writeQueue.empty() || !IsOpen()){
        return;
    }

    auto Self = shared_from_this();

    asio::async_write(
        socket,
        asio::buffer(writeQueue.front()),
        asio::bind_executor(
            strand,
            [Self](const beast::error_code& Error, std::size_t){
                if(Error){
                    Self->Close();
                    Self->NotifyDisconnect();
                    return;
                }

                Self->writeQueue.pop_front();

                if(!Self->writeQueue.empty() && Self->IsOpen()){
                    Self->DoWrite();
                }
            }
        )
    );
}

void ClientConnection::NotifyDisconnect(){
    if(disconnectHandler){
        disconnectHandler(shared_from_this());
    }
}

void ClientConnection::Close(){
    if(!socket.is_open()){
        isActive = false;
        return;
    }

    isActive = false;

    beast::error_code ErrorCode;

    socket.cancel(ErrorCode);
    socket.shutdown(tcp::socket::shutdown_both, ErrorCode);
    socket.close(ErrorCode);

    readBuffer.consume(readBuffer.size());
    writeQueue.clear();
}

bool ClientConnection::Send(
    const std::string& message
){
    if(!IsOpen()){
        return false;
    }

    auto Outgoing = NormalizeMessage(message);
    auto Self = shared_from_this();

    asio::post(
        strand,
        [Self, Outgoing = std::move(Outgoing)]() mutable{
            if(!Self->IsOpen()){
                return;
            }

            bool Writing = !Self->writeQueue.empty();
            Self->writeQueue.push_back(std::move(Outgoing));

            if(!Writing){
                Self->DoWrite();
            }
        }
    );

    return true;
}

void ClientConnection::SetMessageHandler(
    MessageHandler handler
){
    messageHandler = std::move(handler);
}

void ClientConnection::SetDisconnectHandler(
    DisconnectHandler handler
){
    disconnectHandler = std::move(handler);
}

void ClientConnection::SetSessionToken(
    const std::string& token
){
    sessionToken = token;
}

const std::string& ClientConnection::GetSessionToken() const{
    return sessionToken;
}

void ClientConnection::SetUsername(
    const std::string& value
){
    username = value;
}

const std::string& ClientConnection::GetUsername() const{
    return username;
}

bool ClientConnection::IsAuthenticated() const{
    return !sessionToken.empty() && !username.empty();
}