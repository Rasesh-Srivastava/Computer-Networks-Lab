#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <math.h>

int temp2[] = {128, 64, 32, 16, 8, 4, 2, 1};
void binaryToBase64(char *info, int extra, int f, char *answ)
{
	int value;
	for (int i = 0; i < 4; i++)
	{
		if (i >= 4 - extra && f)
		{
			answ[i] = '=';
			continue;
		}
		value = 0;
		for (int j = 0; j < 6; j++)
		{
			value += ((int)(info[i * 6 + j] - '0')) * temp2[j + 2];
		}
		if (value <= 25)
		{
			answ[i] = ((char)(value + 65));
		}
		else if (value <= 51)
		{
			answ[i] = ((char)(value + 71));
		}
		else if (value <= 61)
		{
			answ[i] = ((char)(value - 4));
		}
		else if (value == 62)
		{
			answ[i] = ('+');
		}
		else
		{
			answ[i] = ('/');
		}
	}
}
char *encoder(char *info)
{
	int extra, f = 0;
	int sz = strlen(info);
	extra = 3 - ((sz - 1) % 3) - 1;
	char *answer = (char *)malloc(sizeof(char) * 2 * sz);
	char *tempString = (char *)malloc(sizeof(char) * 4);
	tempString[4] = '\0';
	char *base2 = (char *)malloc(sizeof(char) * 25);
	base2[25] = '\0';
	char *base64 = (char *)malloc(sizeof(char) * 5);
	base64[5] = '\0';
	int i, j;
	for (i = 0, j = 0; i < sz; i += 3, j++)
	{
		if (sz - i <= 3)
		{
			f = 1;
			for (int k = 0; k < 3; k++)
			{
				if (k >= 3 - extra)
					tempString[k] = '\0';
				else
					tempString[k] = info[i + k];
			}
		}
		else
		{
			tempString[0] = info[i + 0];
			tempString[1] = info[i + 1];
			tempString[2] = info[i + 2];
		}
		for (int i = 0; i < 3; i++)
		{
			if (tempString[i] == '\0')
			{
				for (int j = i * 8; j < (i + 1) * 8; j++)
					base2[j] = '0';
			}
			else
			{
				int ascii_value = (int)tempString[i];
				for (int j = 0; j < 8; j++)
				{
					if (ascii_value >= temp2[j])
					{
						ascii_value -= temp2[j];
						base2[i * 8 + j] = '1';
					}
					else
					{
						base2[i * 8 + j] = '0';
					}
				}
			}
		}
		binaryToBase64(base2, extra, f, base64);
		for (int k = 0; k < 4; k++)
		{
			answer[4 * j + k] = base64[k];
		}
	}
	answer[4 * j] = '\0';
	return (answer);
}
