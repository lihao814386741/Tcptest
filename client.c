#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <err.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>

#define BUFFER_SIZE 1024
#define MSG 0
#define EXIT 1


int main()
{
	int sockfd;
	int port;
	struct sockaddr_in dest_addr;
	char ip[16];
	char msg[BUFFER_SIZE];
	FILE *fp = fopen("information_client", "r");

	fscanf(fp, "ip: %s port: %d", ip, &port);

	printf("%s.\n", ip);
	printf("%d.\n", port);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);


	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(port);
	bzero(&(dest_addr.sin_zero), 8);

	inet_pton(AF_INET, ip, &dest_addr.sin_addr.s_addr);
	if (connect(sockfd, (struct sockaddr *)&dest_addr, sizeof (struct sockaddr)) != 0)
		err(1, "WRONG WHEN CONNECT:");

	printf(">>>");
	while (scanf("%s", msg) != EOF)
	{
//		printf("%s\n", msg);
//		
		if (strcmp(msg, "exit") == 0 || strcmp(msg, "quit") == 0)
		{
			int send_flag = EXIT;

			if (write(sockfd, (char *)&send_flag, sizeof(int)) < 0)
				err(1, "send ehlo error:");		

			break;
		}
		else
		{
			int send_flag = MSG;

			if (write(sockfd, (char *)&send_flag, sizeof(int)) < 0)
				err(1, "send ehlo error:");		
		}

		if (write(sockfd, msg, strlen(msg)) < 0)
			err(1, "send ehlo error:");

		char buffer[BUFFER_SIZE];

		int length;
		length = read(sockfd, buffer, sizeof (buffer));
		buffer[length] = 0;
		printf("recv:%s.\n", buffer);
		printf(">>>");
	}

	

	return 0;
}
