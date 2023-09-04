#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#define Port_Number 12345
int main()
{
    int createNewSocket;
    // int n;
    pid_t childpid;
    int sockfd;
    struct sockaddr_in newAddr;
    char tobeSent[1024] = {0};
    char temp[1024] = {0};
    struct sockaddr_in serAdd;
    socklen_t sizeOfAddress;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("Error in connection.\n");
        exit(1);
    }
    else
    {
        printf("Server Socket is created.\n");
    }
    memset(&serAdd, '\0', sizeof(serAdd));
    serAdd.sin_family = AF_INET;
    serAdd.sin_port = htons(Port_Number);
    serAdd.sin_addr.s_addr = inet_addr("127.0.0.1");
    int socketBinder = bind(sockfd, (struct sockaddr *)&serAdd, sizeof(serAdd));
    if (socketBinder < 0)
    {
        printf("Error in binding to port.\n");
        exit(1);
    }
    else
    {
        printf("Bound to port %d\n", Port_Number);
    }
    if (listen(sockfd, 10) == 0)
    {
        printf("Listening to the port %d\n",Port_Number);
    }
    else
    {
        printf("Error in binding.\n");
    }
    while (true)
    {
        createNewSocket = accept(sockfd, (struct sockaddr *)&newAddr, &sizeOfAddress);
        if (createNewSocket < 0)
        {
            exit(1);
        }
        else
        {
        printf("Connection accepted from %s: %d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
        }
        if ((childpid = fork()) == 0)
        {
            close(sockfd);
            while (true)
            {
                recv(createNewSocket, temp, 1024, 0);
                if (strcmp(temp, "/exit") == 0)
                {
                    printf("Disconnected from %s: %d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
                    break;
                }
                else
                {
                    printf("Client: %s\n", temp);
                    printf("Server: ");
                    fgets(tobeSent, sizeof(tobeSent), stdin);
                    tobeSent[strcspn(tobeSent, "\n")] = '\0';
                    send(createNewSocket, tobeSent, strlen(tobeSent), 0);
                    bzero(tobeSent, sizeof(tobeSent));
                    bzero(temp, sizeof(temp));
                }
            }
        }
        else
        {

        }
    }
    close(createNewSocket);
    return 0;
}
