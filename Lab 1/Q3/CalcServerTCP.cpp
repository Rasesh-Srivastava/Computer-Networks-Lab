#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <cmath>
#include <sstream>
#include <arpa/inet.h>
#include <sys/socket.h>
double evaluateExpression(const std::string &inputExpr)
{
    std::istringstream iss(inputExpr);
    char operator1;
    double operand1, operand2;
    iss >> operand1 >> operator1 >> operand2;
    switch (operator1)
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
    int servSoc = socket(AF_INET, SOCK_STREAM, 0);
    if (servSoc == -1)
    {
        std::cerr << "Error creating socket." << std::endl;
        return 1;
    }
    sockaddr_in serAdd;
    serAdd.sin_family = AF_INET;
    serAdd.sin_port = htons(Port_Number);
    serAdd.sin_addr.s_addr = INADDR_ANY;
    if (bind(servSoc, (struct sockaddr *)&serAdd, sizeof(serAdd)) == -1)
    {
        std::cerr << "Binding of server and Port_Number failed." << std::endl;
        return 1;
    }
    if (listen(servSoc, 5) == -1)
    {
        std::cerr << "Error in Listening\n";
        return 1;
    }
    while (1)
    {
        std::cout << "Server listening on Port_Number " << Port_Number << "\n";
        sockaddr_in cliAdd;
        socklen_t cliAddSz = sizeof(cliAdd);
        int cliSoc = accept(servSoc, (struct sockaddr *)&cliAdd, &cliAddSz);
        char cliip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(cliAdd.sin_addr), cliip, INET_ADDRSTRLEN);
        std::cout << "Client: " << cliip << ":" << ntohs(cliAdd.sin_port) << " on Port_Number " << Port_Number << " connected " << std::endl;
        char tempora[1024];
        int dataR;
        while ((dataR = recv(cliSoc, tempora, sizeof(tempora), 0)) > 0)
        {
            tempora[dataR] = '\0';
            std::string inputExpr(tempora);
            std::cout << "Received from client: " << inputExpr << "\n";
            double answ = evaluateExpression(inputExpr);
            std::string answStr = std::to_string(answ);
            if (answStr == "nan")
            {
                answStr = "Not A Number. Invalid Input.";
            }
            std::cout << "Sending to client: " << answStr << "\n";
            send(cliSoc, answStr.c_str(), answStr.size(), 0);
        }
        close(cliSoc);
        std::cout << "Client closed connection.\n";
    }
    close(servSoc);
    return 0;
}
