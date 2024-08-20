#include "OrderManagement.h"
#include "UtilityMethods.h"
#include <iomanip>
#include <sstream>
#include <ctime>
#include <openssl/hmac.h>
#include <openssl/buffer.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/sha.h>
#include <curl/curl.h>
#include <iostream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
using namespace std;

OrderManagement::OrderManagement() 
/* 
* Constructor for the OrderManagement class
*/
{
    loadEnvData();
}

void OrderManagement::loadEnvData() 
/*
* Loads environment variables from a .env file
*/
{
    map<string, string> endData = UtilityMethods::readFileData(".env");
    if (endData.find("CLIENT_ID") != endData.end()) {
		this->clientId = endData["CLIENT_ID"];
	}
	if (endData.find("CLIENT_SECRET") != endData.end()) {
		this->clientSecret = endData["CLIENT_SECRET"];
	}
	if (endData.find("SECRET_PASS") != endData.end()) {
		this->secretPass = endData["SECRET_PASS"];
	}
}

void OrderManagement::submitOrder(const string& orderSide, const string& orderType, const string& instrument, double quantity, double limitPrice)
/*
* Submits an order to the OKX API
*/
{
    try {
        // Determine posSide based on order side
        string posSide = (orderSide == "buy") ? "long" : "short";
        nlohmann::json requestBody;
        requestBody["instId"] = instrument;
        requestBody["tdMode"] = "cross";
        requestBody["side"] = orderSide;
        requestBody["ordType"] = orderType;
        requestBody["sz"] = to_string(floor(quantity));
        requestBody["px"] = to_string(limitPrice);
        requestBody["posSide"] = posSide;
        string serverResponse = makeApiRequest("POST", "/api/v5/trade/order", requestBody.dump());

        auto jsonResponse = nlohmann::json::parse(serverResponse);
        if (jsonResponse.contains("code") && jsonResponse["code"] == "0") cout << "Congrats bro, your order has been placed successfully. Enjoy your time here" << endl;
        else cerr << "Sorry buddy, unable to place your order : " << jsonResponse["msg"] << endl;
    }
    catch (exception e) {
        cout << "Exception occurred in while placing the order : " << e.what() << endl;
    }
}

void OrderManagement::abortOrder(const string& orderId, const string& instrumentId)
/*
* Cancels an order on the OKX API
*/
{
    try {
        // Conditionally add the order ID to the JSON body
        if (orderId.empty() || instrumentId.empty()) {
            cerr << "Sorry bro, unable to cancel order. Pls check your order Id/instrument Id" << endl;
			return;
        }
        nlohmann::json requestBody = {
            { "instId", instrumentId },
            { "ordId", orderId }
        };

        // Sending the cancellation request
        string serverResponse = makeApiRequest("POST", "/api/v5/trade/cancel-order", requestBody.dump());
        if (serverResponse.empty()) {
            cerr << "Failed to cancel order. Empty response received from server" << endl;
            return;
        }

        auto jsonResponse = nlohmann::json::parse(serverResponse);

        if (jsonResponse.contains("code") && jsonResponse["code"] == "0") cout << "Hey buddy, your order " << orderId << " has been cancelled. Try with something diffrent this time" << endl;
        else cerr << "Sorry bro, Order cancellation failed: " << jsonResponse["msg"] << endl;
    }
    catch (const exception& ex) {
        cerr << "Something went wrong while order cancellation : " << ex.what() << endl;
    }
}

void OrderManagement::updateOrder(const string& orderId, const string& instrumentId, const string& newSize, const string& newPrice) 
/*
    Updates an order on the OKX API
*/
{

    try {
        if (orderId.empty() || instrumentId.empty() || newSize.empty() || newPrice.empty()) {
			cerr << "Sorry bro, unable to modify order. Pls check your order Id/instrument Id/provided size/price" << endl;
			return;
		}

        nlohmann::json requestBody = {
            { "ordId", orderId },
            { "instId", instrumentId },
            { "newSz", newSize },
			{ "newPx", newPrice }
        };

        // send POST request
        string serverResponse = makeApiRequest("POST", "/api/v5/trade/amend-order", requestBody.dump());
        if (serverResponse.empty()) {
			cerr << "Failed to update order. Empty response received from server" << endl;
			return;
		}

        auto parsedResponse = nlohmann::json::parse(serverResponse);

        // Validate the response code and display appropriate message
        if (parsedResponse.contains("code") && parsedResponse["code"] == "0") cout << "Congrats bro, your order " << orderId << " is updated successfully." << endl;
        else cerr << "Oops, unable to update order : " << parsedResponse.value("msg", "something went wrong") << endl;
    }
    catch (const exception& ex) {
        cerr << "Something went wrong during order update: " << ex.what() << endl;
    }
}

string OrderManagement::getOrderBook(const std::string& symbol) {
/*
    Retrieves the order book for a given symbol from the OKX API
*/
    try {
        std::string endpoint = "/api/v5/market/books?instId=" + symbol;
        std::string response = makeApiRequest("GET", endpoint, "");
        auto parsedResponse = nlohmann::json::parse(response);
        if (parsedResponse["code"] != "0") {
			cerr << "Sorry bro, we are not able to get the order book : " << parsedResponse["msg"] << endl;
			return "";
		}
        string orderBook = parsedResponse["data"].dump();
        cout << "Congrats, we got the orderbook record for you : " << parsedResponse["data"] << endl;
        return orderBook;
    }
	catch (const std::exception& ex) {
		cerr << "Sorry bro, we are not able to get the order book :  " << ex.what() << endl;
	}
    	return "";
}

string OrderManagement::getCurrentPositions() 
/*
	Retrieves the current positions from the OKX API
*/
{
    try {
        std::string endpoint = "/api/v5/account/positions";
        std::string response = makeApiRequest("GET", endpoint, "");
        auto parsedResponse = nlohmann::json::parse(response);
        if (parsedResponse["code"] != "0") {
            cerr << "Sorry bro, we are not able to get the order book : " << parsedResponse["msg"] << endl;
            return "";
        }
        string currentPos = parsedResponse["data"].dump();
        cout << "Congrats, we got the current positions for you : " << currentPos << endl;
        return currentPos;
	}
	catch (const std::exception& ex) {
		cerr << "Sorry bro, we are not able to get the current positions :  " << ex.what() << endl;
    }
    return "";
}

string OrderManagement::generateSignature(const string& httpMethod, const string& urlPath, const string& timeStamp, const string& requestBody)
/*
* Generates a signature for the OKX API request
*/
{
    try {
        string dataToSign = timeStamp + httpMethod + urlPath + requestBody;
        unsigned char* hmacDigest = HMAC(EVP_sha256(), this->clientSecret.c_str(), this->clientSecret.length(), reinterpret_cast<const unsigned char*>(dataToSign.c_str()), dataToSign.length(), NULL, NULL);
        return UtilityMethods::encodeToBase64(hmacDigest, SHA256_DIGEST_LENGTH);
    }
    catch (exception e) {
        cout << "Exception occurred in generateSignature: " << e.what() << endl;
    }
    return "";
}

string OrderManagement::makeApiRequest(const string& httpMethod, const string& urlPath, const string& payload)
/*
* Makes a request to the OKX API
*/
{
    try {
        CURLcode result;
        CURL* curlHandle;
        string responseBuffer = "";
        curlHandle = curl_easy_init();
        string completeUrl = "https://www.okx.com" + urlPath;

        if (curlHandle) {
            string timeStamp = UtilityMethods::generateISO8601Time();
            if(timeStamp.empty()) {
				cerr << "Failed to generate timestamp" << endl;
				curl_easy_cleanup(curlHandle);
				return "";
			}
            string signature = generateSignature(httpMethod, urlPath, timeStamp, payload);
            if (signature.empty()) {
				cerr << "Failed to generate signature" << endl;
				curl_easy_cleanup(curlHandle);
				return "";
			}
            struct curl_slist* requestHeaders = createRequestHeaders(signature, timeStamp);
            if (requestHeaders == NULL) {
                cerr << "Failed to create request headers" << endl;
                curl_easy_cleanup(curlHandle);
                return "";
            }

            curl_easy_setopt(curlHandle, CURLOPT_HTTPHEADER, requestHeaders);
            curl_easy_setopt(curlHandle, CURLOPT_URL, completeUrl.c_str());
            curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, UtilityMethods::receiveDataCallback);
            curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &responseBuffer);

            if (httpMethod == "POST") curl_easy_setopt(curlHandle, CURLOPT_POSTFIELDS, payload.c_str());

            result = curl_easy_perform(curlHandle);
            if (result != CURLE_OK) cerr << "CURL request failed: " << curl_easy_strerror(result) << endl;
            curl_easy_cleanup(curlHandle);
        }
        return responseBuffer;
    }
    catch (exception e) {
        cout << "Exception occurred in makeApiRequest: " << e.what() << endl;
    }
    return "";
}

struct curl_slist* OrderManagement::createRequestHeaders(const string& signature, const string& timeStamp)
/*
* Creates and returns the list of request headers for the OKX API
*/
{
    struct curl_slist* requestHeaders = NULL;
    try {
        requestHeaders = curl_slist_append(requestHeaders, ("OK-ACCESS-KEY: " + this->clientId).c_str());
        requestHeaders = curl_slist_append(requestHeaders, ("OK-ACCESS-SIGN: " + signature).c_str());
        requestHeaders = curl_slist_append(requestHeaders, ("OK-ACCESS-TIMESTAMP: " + timeStamp).c_str());
        requestHeaders = curl_slist_append(requestHeaders, ("OK-ACCESS-PASSPHRASE: " + this->secretPass).c_str());
        requestHeaders = curl_slist_append(requestHeaders, "Content-Type: application/json");
        requestHeaders = curl_slist_append(requestHeaders, "x-simulated-trading: 1");
    }
    catch (exception& e) {
        cerr << "Exception occurred in createRequestHeaders: " << e.what() << endl;
        if (requestHeaders) {
            curl_slist_free_all(requestHeaders);
            requestHeaders = NULL;
        }
    }
    return requestHeaders;
}

