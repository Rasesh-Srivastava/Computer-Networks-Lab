#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string>
#include <string.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/types.h>

typedef struct packet
{
    char data[1024];
} Packet;
typedef struct frame
{
    int KindOfFrame; // ACK:0, SEQ:1 FIN:2
    int SequenceNumber;
    int acknowledgementNumber;
    int IsThePacketCorrupted;
    Packet packet;
} Frame;

int main(int argc, char *argv[])
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
    char MyBuffer[1024];
    int UniqueIdentifierOfTheFrame = 0;
    Frame FrameToBeSent;
    Frame ReceivedFrame;
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
    ServerAddress.sin_addr.s_addr =inet_addr("127.0.0.1");
    // Connect to the sender
    if (connect(serverSocket, (struct sockaddr *)&ServerAddress, sizeof(ServerAddress)) == -1)
    {
        std::cerr << "Error connecting to the sender." << std::endl;
        return 1;
    }
    else
    {
        printf("Connected to the sender\n\n");
    }
    SizeOfTheAddress = sizeof(NewAddress);
    FrameToBeSent.acknowledgementNumber = 0;
    ReceivedFrame.IsThePacketCorrupted = 0;
    int num_of_packets;
    printf("Enter the number of packets for the simulation: ");
    scanf("%d", &num_of_packets);
    int MyArr[1];
    MyArr[0] = num_of_packets;
    int ErrorChecker = send(serverSocket, MyArr, sizeof(MyArr), 0);
    if (ErrorChecker < 0)
    {
        printf("Error in sending number of packets\n");
        exit(1);
    }
    std::string TemporaryString;
    while (1)
    {
        int SizeOfReceivedFrame = recv(serverSocket, &ReceivedFrame, sizeof(Frame), 0);
        if (SizeOfReceivedFrame > 0 && ReceivedFrame.KindOfFrame == 2)
        {
            printf("\n%d packets received. Closing the connection\n", num_of_packets);
            break;
        }
        if (SizeOfReceivedFrame > 0 && ReceivedFrame.IsThePacketCorrupted == 1)
        {   //if the packet is corrupted
            printf("\nReceived Corrupted Packet. NAK Sent\n");
            TemporaryString = "NAK";
            send(serverSocket, &FrameToBeSent, sizeof(FrameToBeSent), 0);
        }
        else if (SizeOfReceivedFrame > 0 && ReceivedFrame.IsThePacketCorrupted != 1)
        {   //if the packet is not corrupted
            if (ReceivedFrame.SequenceNumber == FrameToBeSent.acknowledgementNumber)
            {   
                if (TemporaryString == "NAK")
                {
                    printf("Packet Received: %s\n", ReceivedFrame.packet.data);
                }
                else
                {
                    printf("\nPacket Received: %s\n", ReceivedFrame.packet.data);
                }
                //if the packet is not corrupted and the sequence number is correct
                FrameToBeSent.SequenceNumber = (ReceivedFrame.SequenceNumber + 1) % 2;
                FrameToBeSent.acknowledgementNumber = (ReceivedFrame.SequenceNumber + 1) % 2;
                FrameToBeSent.IsThePacketCorrupted = rand() % 5;
                send(serverSocket, &FrameToBeSent, sizeof(FrameToBeSent), 0);
                TemporaryString = "ACK";
                printf("ACK Sent.\nWaiting for the next new packet...\n");
                sleep(1);
                UniqueIdentifierOfTheFrame+=1;
            }
            else if (SizeOfReceivedFrame > 0 && ReceivedFrame.SequenceNumber != FrameToBeSent.acknowledgementNumber)
            {
                //if the packet is not corrupted and the sequence number is not correct
                TemporaryString = "Dup";
                printf("Duplicate Packet Received. ACK Sent.\nWaiting for the next new packet...\n");
                sleep(1);
                FrameToBeSent.IsThePacketCorrupted = rand() % 5;
                send(serverSocket, &FrameToBeSent, sizeof(FrameToBeSent), 0);
            }
        }
    }
    close(serverSocket);
    return 0;
}
