#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
using namespace std;

enum State{
  CLIANT,
  SERVER
};
class Server {

  private:
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientLen;
    char buffer[1024];
    const int PORT = 8888;

  public:
    server(){
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket < 0){
            cerr << "Error creating socket." << endl;
            exit(1);
        }

        memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(PORT);

        if (bind(serverSocket, (struct sockaddr*)&serverAddr,sizeof(serverAddr)) < 0)
        {
            cerr << "Bind failed" << endl;
            exit(1);
        }
    }

    void startListening(){
        listen(serverSocket, 1);
        cout << "Server is listening on port " << PORT << "..." << endl;
        clientLen = sizeof(clientAddr);
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr , &clientLen);
        if (clientSocket < 0)
        {
            cerr << "Failed to accept client connection." << endl;
            exit(1);
        }
        cout << "Client connected!" << endl;
    }

    string getOwnIP(){
        char hostbuffer[256];
        char *IPbuffer;
        struct hostent *host_entry;


        gethostname(hostbuffer, sizeof(hostbuffer));
        host_entry = gethostbyname(buffer);
        IPbuffer = inet_ntoa(*((struct in_addr*)host_entry->h_addr_list[0]));
        return string(IPbuffer);
    }

    void sengMessage(const string &message){
        send(clientSocket, message.c_str(), message.length(), 0);
    }

    string receiveMessage(){
        memset(buffer, 0, sizeof(buffer));
        recv(clientSocket, buffer, sizeof(buffer), 0);
        return string(buffer);
    }

    void closeConnection(){
        close(clientSocket);
        close(serverSocket);
        cout << "Connection closed." << endl;
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
