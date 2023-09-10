#include <map>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <iostream>
#include <string.h>
#include <thread>
using namespace std;
map<int, int> myMap;
void GracefullyCloseServer(int serverSocket)
{
    string x;
    getline(cin, x);
    if (x == "/exit")
    {
        printf("Closing the server\n");
        printf("and Exiting.\n");
        exit(0);
    }
}
void HandleMultipleClients(int cliSock)
{
    char myBuffer[1024];
    while (1)
    {
        int NumberOfBytesReceived = recv(cliSock, myBuffer, sizeof(myBuffer), 0);
        if (NumberOfBytesReceived <= 0)
        {
            myMap[cliSock]--;
            printf("Client %d disconnected\n", cliSock);
            break;
        }
        myBuffer[NumberOfBytesReceived] = '\0';
        char myChar = cliSock + '0';
        string theMessage = "User ";
        string temporaryMessage = myBuffer;
        theMessage = theMessage + myChar + ": " + temporaryMessage.substr(2);
        int num = myBuffer[0] - '0';
        if (num <= 9 && num >= 0)
        {
            if (num == cliSock)
            {
                string res1 = "You : " + temporaryMessage.substr(2);
                send(num, res1.c_str(), res1.size(), 0);
            }
            else
            {
                send(num, theMessage.c_str(), theMessage.size(), 0);
            }
        }
        else if (myBuffer[0] == 'A')
        {
            for (auto it : myMap)
            {
                if (it.first == cliSock)
                {
                    string res1 = "You : " + temporaryMessage.substr(2);
                    send(it.first, res1.c_str(), res1.size(), 0);
                }
                else if (it.second > 0)
                {
                    send(it.first, theMessage.c_str(), theMessage.size(), 0);
                }
            }
        }
        else
        {
            string s1 = "Invalid message type/this client does not exist.";
            send(cliSock, s1.c_str(), s1.size(), 0);
        }
    }
    close(cliSock);
}
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <port_number>\n", argv[0]);
        return 1;
    }
    int serverSocket, new_socket, valread;
    struct sockaddr_in server_address;
    char myBuffer[1024] = {0};
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Error in making server socket\n");
        return 1;
    }
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(stoi(argv[1]));
    int nbind = ::bind(serverSocket, (sockaddr *)&server_address, sizeof(server_address));
    if (nbind != -1)
    {
        printf("Server bounded to the port successfully\n");
    }
    else
    {
        printf("Bind failed\n");
        return 1;
    }
    if (listen(serverSocket, 3) >= 0)
    {
        printf("Server is Listening\n");
        printf("Type /exit to close the server and hence, disconnect all the clients.\n");
    }
    else
    {
        printf("Server is not Listening properly at the port => Listening error.\n");
        return 1;
    }
    thread(GracefullyCloseServer, serverSocket).detach();
    while (1)
    {
        struct sockaddr_in clientAddress;
        int addrlen = sizeof(clientAddress);
        if ((new_socket = accept(serverSocket, (struct sockaddr *)&clientAddress, (socklen_t *)&addrlen)) >= 0)
        {
            myMap[new_socket] = myMap[new_socket] + 1;
            printf("Connection successfully accepted from client: ");
            cout << new_socket << endl;
        }
        else
        {
            printf("Error in Accepting the connection request from the Client\n");
            return 1;
        }
        thread clientThread(HandleMultipleClients, new_socket);
        clientThread.detach();
    }
    close(serverSocket);
    return 0;
}