#include "encoder.c"
#include "decoder.c"
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
int main(int argc, char *argv[])
{
	if (argc != 3) {
		printf("Usage: %s <server_hostname> <port_number>\n",argv[0]);
        return 1;
    }
	char *Ip = argv[1];
	struct sockaddr_in server;
	char userChoice[2];
	char *Choice = "Do you want to send a message? If Yes, Type 'y', else Type 'n' and then press Enter. ";
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	char tempor[MSG_LEN + 1];
	if (sockfd == -1)
	{
		perror("Failed to connect to the socket.\n");
		exit(-1);
	}
	else
	{
		printf("Client Socket is successfully created\n");
	}
	server.sin_family = AF_INET;
	if (inet_aton(Ip, &server.sin_addr) == 0)
	{
		write(1, "Error", strlen("Error"));
	}
	int portNum = atoi(argv[2]);
	server.sin_port = htons(portNum);
	socklen_t len = sizeof(struct sockaddr_in);
	if (connect(sockfd, (struct sockaddr *)&server, len) == -1)
	{
		perror("Connection Error");
		exit(-1);
	}
	else
	{
		printf("Connected to the server successfully having socket Address as: ");
		printf(" %s:%d  \n", Ip, portNum);
		dumper();
	}
	char info[MSG_LEN];
	while (true)
	{
		write(1, Choice, strlen(Choice));
		read(0, userChoice, 2);
		if (userChoice[0] == 'n')
		{
			char *base64Encoded_info = encoder("Close this Connection");
			snprintf(tempor, sizeof(tempor), "%c%s", '3', base64Encoded_info);
			write(sockfd, tempor, strlen(tempor));
			write(1, "Type 3 Message has been sent to the server to close the connection", strlen("Type 3 Message has been sent to the server to close the connection"));
			printf(" %s: %d  \n", Ip, portNum);
			dumper();
			write(1, "Connection is successfully closed with the server having the socket address as: ", strlen("Connection is successfully closed with the server having the socket address as: "));
			printf(" %s: %d  \n", Ip, portNum);
			dumper();
			break;
		}
		write(1, "\nEnter the message to be sent: ", strlen("\nEnter the message to be sent: "));
		setValue(info, tempor);
		int i = 0;
		while (true)
		{
			read(0, userChoice, 1);
			if (userChoice[0] == '\n' || i == MSG_LEN - 1)
			{
				break;
			}
			info[i++] = userChoice[0];
		}
		char *base64Encoded_info = encoder(info);
		snprintf(tempor, sizeof(tempor), "%c%s", '1', base64Encoded_info);
		write(sockfd, tempor, strlen(tempor));
		write(1, "Message Type 1, Sent to the server at ", strlen("Message Type 1, Sent to the server at "));
		printf(" %s: %d  \n", Ip, portNum);
		dumper();

		setValue(info, tempor);
		int received = read(sockfd, info, 100);
		strcpy(info + 1, decoder(info + 1));
		if (info[0] != '2')
		{
			printf("Sak  %c",info[0]);
			write(1, "Did Not receive Acknowledgement.\nPlease Resend the message!!\n", strlen("Did Not receive Acknowledgement.\nPlease Resend the message!!\n"));
		}
		else
		{
			write(1, "Acknowledgement message (Type 2) from the server ", strlen("Acknowledgement message (Type 2) from the server "));
			printf(" %s: %d  \n", Ip, portNum);
			dumper();
			write(1, info + 1, strlen(info) - 1);
			write(1, "\n\n", 2);
		}
	}
	close(sockfd);
	return 0;
}
