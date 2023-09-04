#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
int temp[] = {128, 64, 32, 16, 8, 4, 2, 1};
void binaryToASCII(char *mes, int extra, int f, char *answ)
{
	int value;
	for (int i = 0; i < 3; i++)
	{
		if (i >= 3 - extra && f)
		{
			break;
		}
		value = 0;
		for (int j = 0; j < 8; j++)
		{
			value += ((int)(mes[i * 8 + j] - '0')) * temp[j];
		}
		answ[i] = (char)value;
	}
}
char *decoder(char *message)
{
	int extra = 0;
	int f = 0;
	int atTheEnd = 0;
	int sz = strlen(message);
	char *answer = (char *)malloc(sizeof(char) * sz);
	char *tempString = (char *)malloc(sizeof(char) * 5);
	tempString[5] = '\0';
	char *base2 = (char *)malloc(sizeof(char) * 25);
	base2[25] = '\0';
	char *base64 = (char *)malloc(sizeof(char) * 4);
	base64[4] = '\0';
	for (int i = 0, j = 0; i < sz; i += 4, j++)
	{
		for (int k = 0; k < 4; k++)
		{
			if (message[i + k] == '=')
			{
				f = 1;
				extra++;
			}
			tempString[k] = message[i + k];
		}
		for (int i = 0; i < 4; i++)
		{
			if (tempString[i] == '=')
			{
				for (int j = i * 6; j < (i + 1) * 6; j++)
					base2[j] = '0';
			}
			else
			{
				char c = tempString[i];
				int TrueValue;
				if (c >= 'A' && c <= 'Z')
				{
					TrueValue = ((int)(c - 'A'));
				}
				else if (c >= 'a' && c <= 'z')
				{
					TrueValue = ((int)(c - 'a' + 26));
				}
				else if (c >= '0' && c <= '9')
				{
					TrueValue = ((int)(c - '0' + 52));
				}
				else if (c == '+')
				{
					TrueValue = (62);
				}
				else
				{
					TrueValue = (63);
				}
				for (int j = 0; j < 6; j++)
				{
					if (TrueValue >= temp[j + 2])
					{
						TrueValue -= temp[j + 2];
						base2[i * 6 + j] = '1';
					}
					else
					{
						base2[i * 6 + j] = '0';
					}
				}
			}
		}
		binaryToASCII(base2, extra, f, base64);
		for (int k = 0; k < 3; k++)
		{
			if (f && k >= 3 - extra)
			{
				break;
			}
			atTheEnd = 3 * j + k + 1;
			answer[3 * j + k] = base64[k];
		}
	}
	answer[atTheEnd] = '\0';
	return (answer);
}
