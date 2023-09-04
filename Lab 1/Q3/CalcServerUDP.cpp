#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <cmath>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sstream>
double evaluateExpression(const std::string &inputExpr)
{
    std::istringstream iss(inputExpr);
    char operation;
    double operand1, operand2;
    iss >> operand1 >> operation >> operand2;
    switch (operation)
    {
    case '+':
        return operand1 + operand2;
    case '-':
        return operand1 - operand2;
    case '*':
        return operand1 * operand2;
    case '/':
        if (operand2 != 0.0)
        {
            return operand1 / operand2;
        }
        else
        {
            std::cerr << "Division by zero is Not Allowed." << std::endl;
            return NAN;
        }
    case '^':
        return std::pow(operand1, operand2);
    default:
        std::cerr << "Invalid Operation/Input." << std::endl;
        return NAN;
    }
}
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        return 1;
    }
    int Port_Number = std::atoi(argv[1]);
    int serSoc = socket(AF_INET, SOCK_DGRAM, 0);
    if (serSoc == -1)
    {
        std::cerr << "Error creating socket." << std::endl;
        return 1;
    }
    sockaddr_in serAdd;
    serAdd.sin_family = AF_INET;
    serAdd.sin_port = htons(Port_Number);
    serAdd.sin_addr.s_addr = INADDR_ANY;
    if (bind(serSoc, (struct sockaddr *)&serAdd, sizeof(serAdd)) == -1)
    {
        std::cerr << "Binding failed." << std::endl;
        return 1;
    }
    char temporar[1024];
    socklen_t clientAddressLength = sizeof(serAdd);
    std::cout << "Server listening on port: " << Port_Number << "\n";
    while (1)
    {
        int dataR = recvfrom(serSoc, temporar, sizeof(temporar), 0, (struct sockaddr *)&serAdd, &clientAddressLength);
        temporar[dataR] = '\0';
        std::string inputExpr(temporar);
        std::cout << "Received from client: " << inputExpr << "\n";
        double answ = evaluateExpression(inputExpr);
        std::string answStr = std::to_string(answ);
        if (answStr == "nan")
        {
            answStr = "Not A Number. Invalid Input.";
        }
        std::cout << "Sending to client: " << answStr << "\n";
        sendto(serSoc, answStr.c_str(), answStr.size(), 0, (struct sockaddr *)&serAdd, clientAddressLength);
    }
    close(serSoc);
    return 0;
}
