#ifndef Order_Management_H
#define Order_Management_H

#include <string>
using namespace std;

class OrderManagement {
public:
    OrderManagement();
    void loadEnvData();
    string generateSignature(const string& timeStamp, const string& httpMethod, const string& urlPath, const string& requestBody);
    string makeApiRequest(const string& urlPath, const string& httpMethod, const string& payload);
    void submitOrder(const string& instrument, const string& orderSide, const string& orderType, double quantity, double limitPrice);
    void abortOrder(const string& instrumentId, const string& orderId);
    void retrieveOpenOrders(const string& instrumentId, const string& instrumentType);
    void updateOrder(const string& orderId, const string& instrumentId, const string& newQuantity, const string& newPrice);
    string getOrderBook(const std::string& symbol);
    string getCurrentPositions();
    struct curl_slist* createRequestHeaders(const string& signature, const string& timeStamp);

private:
    std::string clientId;
    std::string clientSecret;
    std::string secretPass;
};

#endif // OKX_CLIENT_H