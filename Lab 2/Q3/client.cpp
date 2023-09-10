#include <map>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include <arpa/inet.h>
#include <string.h>
#include <thread>
using namespace std;
int MyCounter = 0;
void SendMessage(int cliSock)
{
    while (1)
    {
        string myMessage;
        getline(cin, myMessage);
        if (myMessage == "/exit")
        {
            MyCounter = 1;
            string TheExitMessage = "A User closed the connection and exited";
            send(cliSock, TheExitMessage.c_str(), TheExitMessage.length(), 0);
            break;
        }
        send(cliSock, myMessage.c_str(), myMessage.length(), 0);
    }
}
int main(int argc, char *argv[])
{
    struct sockaddr_in ServerAddress;
    int status;
    int cliSock;
    if (argc != 3)
    {
        printf("Usage: %s <server_hostname> <port_number>\n", argv[0]);
        return 1;
    }
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_addr.s_addr = inet_addr(argv[1]);
    ServerAddress.sin_port = htons(stoi(argv[2]));
    char buffer[1024] = {0};
    if ((cliSock = socket(AF_INET, SOCK_STREAM, 0)) >= 0)
    {
        printf("Socket created successfully\n");
    }
    else
    {
        printf("\n Socket is not created successfully => Error in Socket Creation.\n");
        return 1;
    }
    if ((status = connect(cliSock, (struct sockaddr *)&ServerAddress, sizeof(ServerAddress))) >= 0)
    {
        printf("Connected to the server successfully\n");
        printf("Welcome! You can start sending messages now.\n");
        printf("Type <A 'message'> to send the message to everyone in the chat room (broadcast).\n");
        printf("Type <<User ID> 'message'> to send the message to the specific User (private chat).\n");
        printf("Type /exit to close the connection\n");
    }
    else
    {
        printf("\nConnection to the server failed\n");
        return -1;
    }
    thread sendmess(SendMessage, cliSock);
    sendmess.detach();
    while (1)
    {
        if ((MyCounter - 1) == 0)
        {
            return 0;
        }
        char MyAcknowledgementBuff[1024];
        int NumberOfBytesReceived = recv(cliSock, MyAcknowledgementBuff, sizeof(MyAcknowledgementBuff), 0);
        if (NumberOfBytesReceived > 0)
        {
            MyAcknowledgementBuff[NumberOfBytesReceived] = '\0';
            cout << MyAcknowledgementBuff << endl;
        }
        else if (NumberOfBytesReceived <= 0)
        {
            printf("Connection is closed by the server. Thank you :)\n");
            exit(0);
            break;
        }
        if ((MyCounter - 1) == 0)
        {
            return 0;
        }
    }
    return 0;
}