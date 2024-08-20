#include "WebsocketConnection.h"
#include "OrderManagement.h"

WebsocketConnection::WebsocketConnection()
/*
    Constructor
*/
{
    wsServer_.init_asio();
    wsServer_.set_open_handler(std::bind(&WebsocketConnection::onOpen, this, std::placeholders::_1));
    wsServer_.set_close_handler(std::bind(&WebsocketConnection::onClose, this, std::placeholders::_1));
    wsServer_.set_message_handler(std::bind(&WebsocketConnection::onMessage, this, std::placeholders::_1, std::placeholders::_2));
}

WebsocketConnection::~WebsocketConnection()
/*
    Destructor
*/
{
    stopServer();
}

void WebsocketConnection::startServer(unsigned short port) 
/*
	Start the websocket server on the specified port
*/
{
    wsServer_.listen(port);
    wsServer_.start_accept();

    // Start a separate thread for periodic order book updates
    orderBookUpdateThread_ = std::thread(&WebsocketConnection::sendOrderBookUpdates, this);
    wsServer_.run();
}

void WebsocketConnection::stopServer() 
/*
	Stop the websocket server
*/
{
    wsServer_.stop_listening();
    wsServer_.stop();

    // Stop the order book update thread
    if (orderBookUpdateThread_.joinable()) {
        orderBookUpdateThread_.join();
    }
}

void WebsocketConnection::onOpen(websocketpp::connection_hdl hdl) {
    // Handle new connection
}

void WebsocketConnection::onClose(websocketpp::connection_hdl hdl) {
    // Handle disconnection
}

void WebsocketConnection::onMessage(websocketpp::connection_hdl hdl, websocketpp::server<websocketpp::config::asio>::message_ptr msg) 
/*
    Handle incoming messages from clients
*/
{
    std::string message = msg->get_payload();

    // Example: Extract symbol from the message and handle subscription
    this->symbol = message; // Simplified extraction
    subscribers_[this->symbol] = hdl;

    // You can send a confirmation message back to the client
    wsServer_.send(hdl, "Subscribed to " + this->symbol, websocketpp::frame::opcode::text);
}

void WebsocketConnection::sendMessage(const std::string& message) 
/*
	Send a message to all subscribers of the symbol
*/
{
    try {
        auto it = subscribers_.find(this->symbol);
        if (it != subscribers_.end()) {
            wsServer_.send(it->second, message, websocketpp::frame::opcode::text);
        }
    } catch (const std::exception& e) {
		std::cerr << "Exception in sendMessage: " << e.what() << std::endl;
	}
}

void WebsocketConnection::sendOrderBookUpdates() 
/*
	Send periodic order book updates to all subscribers
*/
{
    try {
        OrderManagement orderManagement;
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(5));

            if (!this->symbol.empty()) {
                //std::string symbol = "BTC-USDT-SWAP";
                std::string orderBookData = orderManagement.getOrderBook(this->symbol);
                // Send the order book data to all subscribers of the symbol
                sendMessage(orderBookData);
            }
        }
	} catch (const std::exception& e) {
		std::cerr << "Exception in sendOrderBookUpdates: " << e.what() << std::endl;
    }
}
