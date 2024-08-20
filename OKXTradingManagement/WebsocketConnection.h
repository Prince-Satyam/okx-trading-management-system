#pragma once
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <unordered_map>
#include <string>
#include <thread>

class WebsocketConnection {
public:
    WebsocketConnection();
    ~WebsocketConnection();

    void startServer(unsigned short port);
    void stopServer();

private:
    void onOpen(websocketpp::connection_hdl hdl);
    void onClose(websocketpp::connection_hdl hdl);
    void onMessage(websocketpp::connection_hdl hdl, websocketpp::server<websocketpp::config::asio>::message_ptr msg);

    void sendMessage(const std::string& message);
    void sendOrderBookUpdates();

    websocketpp::server<websocketpp::config::asio> wsServer_;
    std::unordered_map<std::string, websocketpp::connection_hdl> subscribers_;
    std::thread orderBookUpdateThread_;
    std::string symbol;
};
