#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string>
#include <iostream>
#include <netinet/in.h>
#include <sys/types.h>
#include <time.h>
typedef struct packet
{
    char data[1024];
} Packet;
typedef struct Num
{
    int num;
} MyNum;
typedef struct frame
{
    int KindOfFrame; // ACK:0, SEQ:1 FIN:2
    int SequenceNumber;
    int acknowledgementNumber;
    int IsThePacketCorrupted;
    Packet packet;
} Frame;
int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        return 1;
    }
    int port = atoi(argv[1]);
    int serverSocket;
    struct sockaddr_in ServerAddress, NewAddress;
    socklen_t SizeOfTheAddress;
    int UniqueIdentifierOfTheFrame = 0;
    char MyBuffer[1024];
    Frame ReceivedFrame;
    Frame FrameToBeSent;
    int IsACKreceived = 1;
    // Create a TCP socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
    {
        std::cerr << "Error creating socket." << std::endl;
        return 1;
    }
    else
    {
        printf("Socket Created Successfully\n");
    }
    // Bind the socket to a port
    memset(&ServerAddress, '\0', sizeof(ServerAddress));
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_port = htons(port);
    ServerAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    if (bind(serverSocket, (struct sockaddr *)&ServerAddress, sizeof(ServerAddress)) == -1)
    {
        std::cerr << "Binding failed." << std::endl;
        return 1;
    }
    else
    {
        printf("Binding Successful\n");
    }
    // Listen for connections
    if (listen(serverSocket, 5) == -1)
    {
        std::cerr << "Error listening." << std::endl;
        return 1;
    }
    else
    {
        printf("Server Listening on port number %d...\n", port);
    }
    FrameToBeSent.KindOfFrame = 1;
    ReceivedFrame.IsThePacketCorrupted = 0;
    int num_of_packets;
    int MyArr[1];
    // Accept a connection
    int clientSocket = accept(serverSocket, (struct sockaddr *)&NewAddress, &SizeOfTheAddress);
    if(clientSocket == -1)
    {
        std::cerr << "Error accepting connection." << std::endl;
        return 1;
    }
    else
    {
        printf("Connection Request from the receiver Accepted\n\n");
    }
    // Receive the number of packets
    int ErrorChecker = recv(clientSocket, MyArr, sizeof(MyArr), 0);
    if (ErrorChecker < 0)
    {
        printf("Error in receiving the number of packets\n");
        exit(1);
    }
    // Send the number of packets
    num_of_packets = MyArr[0];
    printf("Total number of packets to be sent = %d\n", num_of_packets);
    int MyCounter = 0;
    std::string TemporaryString;
    // Send the packets
    while (MyCounter < num_of_packets || TemporaryString != "ACK")
    {
        if (IsACKreceived == 1)
        {
            FrameToBeSent.SequenceNumber = UniqueIdentifierOfTheFrame % 2;
            FrameToBeSent.acknowledgementNumber = 0;
            if (FrameToBeSent.KindOfFrame == 1)
            {
                printf("\n");
                sleep(2);
                std::string s = "Message " + std::to_string(MyCounter + 1) + " with sequence number " + std::to_string(FrameToBeSent.SequenceNumber);
                // Copy the string to the buffer
                strcpy(MyBuffer, s.c_str());
                MyCounter++;
            }
            // Generate a random number to decide whether the packet is corrupted or not
            FrameToBeSent.IsThePacketCorrupted = rand() % 5;
            // Send the packet
            strcpy(FrameToBeSent.packet.data, MyBuffer);
            send(clientSocket, &FrameToBeSent, sizeof(Frame), 0);
            printf("Message %d Sent with sequence number %d\n", MyCounter, FrameToBeSent.SequenceNumber);
            printf("Waiting for ACK...\n");
            sleep(2);
        }
        int SizeOfReceivedFrame = recv(clientSocket, &ReceivedFrame, sizeof(ReceivedFrame), 0);
        if (SizeOfReceivedFrame > 0 && (ReceivedFrame.IsThePacketCorrupted == 1))
        {
            // If the packet is corrupted
            TemporaryString = "Corrupt";
            printf("Received Corrupted Packet. Sending the same packet again.\n");
            IsACKreceived = 1;
            FrameToBeSent.KindOfFrame = 0;
        }
        else if (SizeOfReceivedFrame > 0)
        {
            // If the packet is not corrupted and the sequence number is equal to the acknowledgement number
            if (ReceivedFrame.SequenceNumber == UniqueIdentifierOfTheFrame % 2)
            {
                TemporaryString = "NAK";
                IsACKreceived = 1;
                printf("Received NAK. Sending the same packet again\n");
                FrameToBeSent.KindOfFrame = 0;
            }
            else if (ReceivedFrame.SequenceNumber != UniqueIdentifierOfTheFrame % 2)
            {
                // If the packet is not corrupted and the sequence number is not equal to the acknowledgement number
                TemporaryString = "ACK";
                printf("ACK Received\n");
                FrameToBeSent.KindOfFrame = 1;
                UniqueIdentifierOfTheFrame++;
                IsACKreceived = 1;
            }
        }
    }
    printf("\nAcknowledgment of all %d packets received. Closing the connection\n", num_of_packets);
    FrameToBeSent.KindOfFrame = 2;
    send(clientSocket, &FrameToBeSent, sizeof(Frame), 0);
    // Close the socket
    close(clientSocket);
    close(serverSocket);
    sleep(1);
    printf("Connection Closed\n");
    printf("Simulation Complete\n");
    return 0;
}
