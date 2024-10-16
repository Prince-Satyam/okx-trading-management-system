
# OKX Trade Management System

This is a C++ project for order execution and management to trade on OKX. It will have below functionalities:

1. Place order
2. Cancel order
3. Modify order
4. Get orderbook
5. View current positions
6. Create a websocket server that clients can connect to and subscribe to a symbol by sending a message
7. The server should respond with a stream of messages every second with the orderbook updates for each symbol that is subscribed to


### - External packages used:
1. OpenSSL
2. nlohmann
3. curl
4. websocketpp

### - Installation procedure:
1. Clone repo 
2. Install dependencies :
- If vcpkg is installed then go to the main directory of vcpkg installation (for eg - D:\C++Libs\vcpkg), open cmd and enter commands: `vcpkg install openssl nlohmann-json curl websocketpp`
- If vcpkg is not installed then follow the link to install the vcpkg and repeat above step: https://learn.microsoft.com/en-us/vcpkg/get_started/get-started?pivots=shell-cmd
3. Create '.env' with structure:
- `CLIENT_ID = user api key`
- `CLIENT_SECRET = user secret key`
- `SECRET_PASS = user password`
4. Open the project in Visual Studio
5. Build the project
6. Run the project
7. Comment out the functions which you do not want to execute as per the description in the cpp files

### - Video description link:
https://drive.google.com/file/d/1GTZ96Wz8JapJCp7GMrrXYguYkCNjMEcG/view?usp=sharing
