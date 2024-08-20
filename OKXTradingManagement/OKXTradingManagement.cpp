// OKXTradingManagement.cpp : Program execution begins and ends there.

#include <iostream>
#include "OrderManagement.h"
#include "WebsocketConnection.h"
#include <iostream>
#include <thread>
using namespace std;

void websocketManagement() {
    WebsocketConnection server;

    // Start the WebSocket server on a separate thread
    std::thread serverThread([&server]() {
        server.startServer(9002); // Start the server on port 9002
        });

    // Keep the main thread alive to let the server run
    std::cout << "Press Enter to stop the server..." << std::endl;
    std::cin.get();

    // Stop the server and join the thread before exiting
    server.stopServer();
    serverThread.join();
}

int main()
/*
    Main function for OrderManagement class
*/
{
	OrderManagement orderManagement;
	//orderManagement.submitOrder("buy", "limit", "BTC-USDT-SWAP", 2, 0.5);
	//orderManagement.abortOrder("1733888209401331712", "BTC-USDT-SWAP");
	//orderManagement.updateOrder("1733828170724589568", "BTC-USDT-SWAP", "8", "0.8");
	//orderManagement.getOrderBook("BTC-USDT-SWAP");
	//orderManagement.getCurrentPositions();

    websocketManagement();

    return 0;
}

