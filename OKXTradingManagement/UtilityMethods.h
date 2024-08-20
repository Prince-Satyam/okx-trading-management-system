#ifndef Utility_Method_H
#define Utility_Method_H
#include <string>
#include <map>
using namespace std;

class UtilityMethods
{
	public:
		static string encodeToBase64(const unsigned char* input, size_t length);
		static size_t receiveDataCallback(void* buffer, size_t elementSize, size_t elementCount, void* userData);
		static map<string, string> readFileData(const string& filePath);
		static string generateISO8601Time();
};

#endif // Utility_Method_H