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
    int ret;
    int cliSoc;
    struct sockaddr_in serAdd;
    char temp[1024] = {0};
    char temp2[1024] = {0};
    cliSoc = socket(AF_INET, SOCK_STREAM, 0);
    if (cliSoc < 0)
    {
        printf("Error in connection.\n");
        exit(1);
    }
    printf("Client Socket is created.\n");

    memset(&serAdd, '\0', sizeof(serAdd));
    serAdd.sin_family = AF_INET;
    serAdd.sin_port = htons(Port_Number);
    serAdd.sin_addr.s_addr = inet_addr("127.0.0.1");

    ret = connect(cliSoc, (struct sockaddr *)&serAdd, sizeof(serAdd));
    if (ret < 0)
    {
        printf("Error in connection.\n");
        exit(1);
    }
    printf("Connected to Server.\n");
    while (true)
    {
        printf("Maximum Length of Message = 1023 characters. Type '/exit' and press Enter to exit.\n");
        printf("Client Message:  ");
        fgets(temp, sizeof(temp), stdin);
        temp[strcspn(temp, "\n")] = '\0';
        send(cliSoc, temp, strlen(temp), 0);
        if (strcmp(temp, "/exit") == 0)
        {
            close(cliSoc);
            printf("Disconnected from the server.\n");
            exit(1);
        }
        if (recv(cliSoc, temp2, sizeof(temp2), 0) < 0)
        {
            printf("Error in receiving data from the server.\n");
        }
        else
        {
            printf("Server Message:  %s\n", temp2);
            memset(temp2, 0, sizeof(temp2));
        }
    }
    return 0;
}
