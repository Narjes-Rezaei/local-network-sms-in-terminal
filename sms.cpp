#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>

class Server {
private:
    int serverSocket = -1;
    int clientSocket = -1;
    sockaddr_in serverAddr{}, clientAddr{};
    socklen_t clientLen{};
    const int PORT = 12345;

public:
    Server() {
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == -1)
            throw std::runtime_error("Error creating socket");

        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(PORT);

        if (bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) < 0)
            throw std::runtime_error("Error binding");

        std::cout << "serverSocket is created and connected on port: " << std::endl;
    }

    void startListening() {
        if (listen(serverSocket, 1) < 0)
            throw std::runtime_error("Error listening");

        std::cout << "listening on port: " << PORT << "..." << std::endl;
        clientLen = sizeof(clientAddr);
        clientSocket = accept(serverSocket, reinterpret_cast<sockaddr*>(&clientAddr), &clientLen);
        if (clientSocket < 0)
            throw std::runtime_error("Error accepting");

        std::cout << "client connected" << std::endl;
    }

    std::string getOwnIP() {
        char hostname[256];
        if (gethostname(hostname, sizeof(hostname)) != 0)
            return "UnknownHost";

        hostent* host = gethostbyname(hostname);
        if (!host) return "UnknownIP";

        return std::string(inet_ntoa(*reinterpret_cast<in_addr*>(host->h_addr_list[0])));
    }

    void sendMessage(const std::string& msg) {
        if (send(clientSocket, msg.c_str(), msg.size(), 0) == -1)
            std::cerr << "Error sending message" << std::endl;
    }

    std::string receiveMessage() {
        std::vector<char> buffer(1024, 0);
        ssize_t bytesReceived = recv(clientSocket, buffer.data(), buffer.size(), 0);
        if (bytesReceived <= 0) {
            return "";
        }
        return std::string(buffer.data(), bytesReceived);
    }

    void closeConnection() {
        if (clientSocket != -1) close(clientSocket);
        if (serverSocket != -1) close(serverSocket);
        std::cout << "connection closed" << std::endl;
    }
};

class Client {
private:
    int clientSocket;
    sockaddr_in serverAddr;

public:
    Client() {
        clientSocket = -1;
        std::memset(&serverAddr, 0, sizeof(serverAddr));
    }

    void connectToServer(const std::string& ip, int port) {
        clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == -1)
            throw std::runtime_error("Error creating client socket");

        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);

        if (inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr) <= 0)
            throw std::runtime_error("Invalid IP address");

        if (connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) < 0)
            throw std::runtime_error("Failed to connect to server");

        std::cout << "Connected to server at " << ip << ":" << port << std::endl;
    }

    void sendMessage(const std::string& message) {
        if (send(clientSocket, message.c_str(), message.size(), 0) == -1)
            std::cerr << "Error sending message" << std::endl;
    }

    std::string receiveMessage() {
        std::vector<char> buffer(1024, 0);
        ssize_t bytesReceived = recv(clientSocket, buffer.data(), buffer.size(), 0);
        if (bytesReceived <= 0) {
            return "";
        }
        return std::string(buffer.data(), bytesReceived);
    }

    void closeConnection() {
        if (clientSocket != -1) {
            close(clientSocket);
            clientSocket = -1;
            std::cout << "Connection closed" << std::endl;
        }
    }
};


class Interface{
  public:

  private:
};

int main(int argc, char* argv[]) {
    try {
        Server server;
        server.startListening();
        server.sendMessage("its ok");
        server.getOwnIP();
        std::cout << server.receiveMessage();
        server.closeConnection();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}
