#include "UtilityMethods.h"
#include <openssl/hmac.h>
#include <openssl/buffer.h>
#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <chrono>
#include <sstream>
#include <iomanip>

using namespace std;

map<string, string> UtilityMethods::readFileData(const string& filePath)
/*
	Reads data from a file and returns a map of key-value pairs
*/
{
	map<string, string> envMap;
	try {
		ifstream file(filePath);
		string line;

		while (getline(file, line)) {
			size_t equalSignPos = line.find('=');
			if (equalSignPos != string::npos) {
				string key = line.substr(0, equalSignPos);
				string value = line.substr(equalSignPos + 1);
				envMap[key] = value;
			}
		}
	}
	catch (exception e) {
		cout << "Exception occurred in loadEnvFile: " << e.what() << endl;
	}

	return envMap;
}

string UtilityMethods::encodeToBase64(const unsigned char* input, size_t length)
/*
	Encodes a byte array to a base64 string
*/
{
	BIO* b64Filter, * bufferMem;
	try {
		BUF_MEM* bufferPtr;
		b64Filter = BIO_new(BIO_f_base64());
		BIO_set_flags(b64Filter, BIO_FLAGS_BASE64_NO_NL); // Suppress newlines
		bufferMem = BIO_new(BIO_s_mem());
		b64Filter = BIO_push(b64Filter, bufferMem);
		BIO_write(b64Filter, input, length);
		BIO_flush(b64Filter);
		BIO_get_mem_ptr(b64Filter, &bufferPtr);

		string encodedOutput(bufferPtr->data, bufferPtr->length);
		BIO_free_all(b64Filter);
		return encodedOutput;
	}
	catch (exception e) {
		cout << "Exception occurred in encodeToBase64: " << e.what() << endl;
		BIO_free_all(b64Filter);
	}
	return "";

}

size_t UtilityMethods::receiveDataCallback(void* buffer, size_t elementSize, size_t elementCount, void* userData)
/*
	Callback function for receiving data from a CURL request
*/
{
	try {
		size_t fullSize = elementSize * elementCount;
		string* outputString = (string*)userData;
		outputString->append((char*)buffer, fullSize);
		return fullSize;
	}
	catch (exception e) {
		cout << "Exception occurred in receiveDataCallback: " << e.what() << endl;
	}
	return 0;
}

string UtilityMethods::generateISO8601Time()
/*
	Generates a timestamp in ISO 8601 format
*/
{
	try {
		using namespace chrono;
		auto currentTime = system_clock::now();
		auto seconds = system_clock::to_time_t(currentTime);
		tm timeStruct = {};
		gmtime_s(&timeStruct, &seconds);
		auto millisec = duration_cast<milliseconds>(currentTime.time_since_epoch()) % 1000;
		ostringstream oss;
		oss << put_time(&timeStruct, "%Y-%m-%dT%H:%M:%S")
			<< '.' << setw(3) << setfill('0') << millisec.count()
			<< 'Z';
		return oss.str();
	}
	catch (exception e) {
		cout << "Exception occurred in generateISO8601Time: " << e.what() << endl;
	}
	return "";
}