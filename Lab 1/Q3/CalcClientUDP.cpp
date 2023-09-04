#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <iostream>
#include <netdb.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fstream>
using namespace std;
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <server_hostname> <port>" << std::endl;
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
    int cliSoc = socket(AF_INET, SOCK_DGRAM, 0);
    if (cliSoc == -1)
    {
        std::cerr << "Error creating socket." << std::endl;
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
    char temporar[1024];
    socklen_t serAddLen = sizeof(serAdd);
    cout << "UDP Client connected to " << serName << " ";
    cout << "on port: " << Port_Number << "\n";
    while (1)
    {
        cout << "Enter an expression in the following format:\noperand1 operator operand2. For example, 3 - 5\nValid operators are + - * / ^\nor '-1' to exit: ";
        string inputStr;
        getline(std::cin, inputStr);
        if (inputStr == "-1")
        {
            break;
        }
        sendto(cliSoc, inputStr.c_str(), inputStr.size(), 0, (struct sockaddr *)&serAdd, serAddLen);
        int dataR = recvfrom(cliSoc, temporar, sizeof(temporar), 0, (struct sockaddr *)&serAdd, &serAddLen);
        temporar[dataR] = '\0';
        cout << "Answer from the server: " << temporar << "\n";
        cout << "\n";
    }
    close(cliSoc);
    return 0;
}
