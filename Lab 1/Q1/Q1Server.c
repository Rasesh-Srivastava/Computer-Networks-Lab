#include "decoder.c"
#include "encoder.c"
#define MSG_LEN 5000
#include <stdbool.h>
void dumper()
{
	fflush(stdout);
}
void setValue(char *tempor1, char *tempor2)
{
	int i;
	for (i = 0; i < MSG_LEN; i++)
	{
		tempor1[i] = '\0';
		tempor2[i] = '\0';
	}
	tempor2[i] = '\0';
}
void Manager(int sockfd, struct sockaddr_in *client)
{
	char info[MSG_LEN];
	char *IPAddress = inet_ntoa(client->sin_addr);
	int Port_Number = client->sin_port;
	char buffer[MSG_LEN + 1];
	printf("Connection Established between the server and the client having socket address: ");
	printf("%s:%d\n", IPAddress, Port_Number);
	dumper();
	while (true)
	{
		setValue(info, buffer);
		int st = read(sockfd, info, MSG_LEN);
		write(1, "\n", 1);
		if (info[0] == '3')
		{
			write(1, "To close the connection, Message of Type 3 received from the client ", strlen("To close the connection, Message of Type 3 received from the client "));
			printf("%s: %d\n", IPAddress, Port_Number);
			dumper();
			write(1, "Connection is successfully closed with the client having the socket address: ", strlen("Connection is successfully closed with the client having the socket address: "));
			printf("%s: %d\n", IPAddress, Port_Number);
			dumper();
			write(1, "\n", 1);
			break;
		}
		write(1, "Message of Type 1 received from the client ", strlen("Message of Type 1 received from the client "));
		printf("%s: %d\n", IPAddress, Port_Number);
		dumper();
		printf("%s: %d - Encoded Message: ", IPAddress, Port_Number);
		dumper();
		write(1, info + 1, st - 1);
		write(1, "\n", 1);
		char *decoded_message = decoder(info + 1);
		printf("%s: %d - Decoded Message: ", IPAddress, Port_Number);
		dumper();
		write(1, decoded_message, strlen(decoded_message));
		write(1, "\n", 1);
		char *ack = encoder("Message: Acknowledgement (Type 2) message");
		snprintf(buffer, sizeof(buffer), "%c%s", '2', ack);
		write(sockfd, buffer, strlen(buffer));
		write(1, "Message Type 2, Sent to the client: ", strlen("Message Type 2, Sent to the client: "));
		printf("%s: %d\n", IPAddress, Port_Number);
		dumper();
	}
	close(sockfd);
	exit(0);
}
int main(int argc, char *argv[])
{
	if (argc != 2) {
		printf("Usage: %s <port_number>\n",argv[0]);
        return 1;
    }
	struct sockaddr_in client, server;
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		perror("Failure to create socket\n");
		exit(-1);
	}
	else
	{
		printf("Socket is successfully created\n");
	}
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(atoi(argv[1]));
	bzero(&server.sin_zero, 8);
	socklen_t len = sizeof(struct sockaddr_in);
	if (bind(sockfd, (struct sockaddr *)&server, len) == -1)
	{
		perror("Failure to bind Port and socket");
		exit(-1);
	}
	else
	{
		printf("Socket is successfully bound to the Port_Number\n");
	}

	if (listen(sockfd, 5) == -1)
	{
		perror("Server is not listening due to listening failure");
		exit(-1);
	}
	else
	{
		printf("Server is listening\n");
	}
	while (true)
	{
		int userSocket = accept(sockfd, (struct sockaddr *)&client, &len);
		if (userSocket == -1)
		{
			perror("Connection Failure");
			exit(-1);
		}
		int theFlag = fork();
		if (theFlag == 0)
		{
			close(sockfd);
			Manager(userSocket, &client);
			break;
		}
		else if (theFlag == -1)
		{
			printf("Unable to fork due to a forking error\n");
			break;
		}
		else
		{
			close(userSocket);
		}
	}
	return 0;
}
