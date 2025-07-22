
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cstring>

class Server {
private:
    int serverSocket = -1;
    int clientSocket = -1;
    sockaddr_in serverAddr{}, clientAddr{};
    socklen_t clientLen{};
    const int PORT = 8080;

public:
    Server() {
        // ساخت سوکت TCP
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == -1)
            throw std::runtime_error("Error creating socket");

        // تنظیمات آدرس سرور
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(PORT);

        // اتصال سوکت به پورت
        if (bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) < 0)
            throw std::runtime_error("Error binding");

        std::cout << "serverSocket is created and connected on port" << endl;
    }

    void startListening() {
        if (listen(serverSocket, 1) < 0)
            throw std::runtime_error("Error listening");

        std::cout << "listening on port: " << PORT << "..." << endl;
        clientLen = sizeof(clientAddr);
        clientSocket = accept(serverSocket, reinterpret_cast<sockaddr*>(&clientAddr), &clientLen);
        if (clientSocket < 0)
            throw std::runtime_error("Error ecceptig");

        std::cout << "client connected" << endl;
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
            std::cerr << "Error sending message" << endl;
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
        std::cout << "connection closed" << endl;
    }
};

class Client {
  public:

  private:
};


class Interface{
  public:

  private:
};

int main (int argc, char *argv[]) {
  
  return 0;
}
