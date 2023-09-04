#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string>
#include <netdb.h>
#include <sys/param.h>
#include <netinet/in.h>
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <server_hostname> <port>\n";
        return 1;
    }
    hostent *record = gethostbyname(argv[1]);
    if (record == NULL)
    {
        printf("%s is unavailable\n", argv[1]);
        exit(1);
    }
    in_addr *address = (in_addr *)record->h_addr;
    char *serName = inet_ntoa(*address);
    int Port_Number = std::atoi(argv[2]);
    int cliSoc = socket(AF_INET, SOCK_STREAM, 0);
    if (cliSoc == -1)
    {
        std::cerr << "Error in creating the socket." << std::endl;
        return 1;
    }
    sockaddr_in serAdd;
    serAdd.sin_family = AF_INET;
    serAdd.sin_port = htons(Port_Number);
    if (inet_pton(AF_INET, serName, &serAdd.sin_addr) <= 0)
    {
        std::cerr << "Invalid address." << std::endl;
        return 1;
    }
    if (connect(cliSoc, (struct sockaddr *)&serAdd, sizeof(serAdd)) == -1)
    {
        std::cerr << "Connection failed." << std::endl;
        return 1;
    }
    char tempor[1024];
    int dataR;
    std::cout << "TCP Client connected to " << serName << " ";
    std::cout << "on port: " << Port_Number << "\n";
    while (true)
    {
        std::cout << "Enter an expression in the following format:\noperand1 operator operand2\nValid operators are + - * / ^\nor '-1' to exit: ";
        std::string inputExpr;
        std::getline(std::cin, inputExpr);
        if (inputExpr == "-1")
        {
            break;
        }
        send(cliSoc, inputExpr.c_str(), inputExpr.size(), 0);
        dataR = recv(cliSoc, tempor, sizeof(tempor), 0);
        tempor[dataR] = '\0';
        std::cout << "Answer from the server: " << tempor << std::endl;
        std::cout << std::endl;
    }
    close(cliSoc);
    return 0;
}
