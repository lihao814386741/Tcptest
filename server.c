#include <string.h> 
#include <stdio.h>
#include <stdlib.h>
#include <err.h> 
#include <sys/socket.h> 
#include <sys/types.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <sys/select.h>


#define MAX_THREAD 1024
#define BUFFER_SIZE 1024
#define TIME_LENGTH 40
#define BACKLOG 5 
#define MSG 0
#define EXIT 1


int sockfd;
void get_time(char time_string[TIME_LENGTH])
{
	time_t t;
	time(&t);
	sprintf(time_string, "%s", ctime(&t));
}
void rec(void *arg)
{

	int *new_fd = ((void **)arg)[0];
	struct sockaddr_in *their_addr = ((void **)arg)[1];
	char buffer[BUFFER_SIZE];
	FILE *fp;
	fp = fopen("log", "a+");

	char time_string[TIME_LENGTH];

	get_time(time_string);


	fprintf(fp, "[%s] is comming at %s", inet_ntoa(their_addr -> sin_addr), time_string);
	fflush(fp);
	printf("[%s] is comming at %s", inet_ntoa(their_addr -> sin_addr), time_string);
	int n;
	
	while (1)
	{
		int recv_flag;
		
		recv(*new_fd, (char *)&recv_flag, sizeof (int), 0);


		if (recv_flag == MSG)
		{
			n = recv(*new_fd, buffer, BUFFER_SIZE, 0);
			buffer[n] = 0;

			int i;
			for (i = 0; buffer[i] != 0; ++ i)
			{
				if (buffer[i] <= 'z' && buffer[i] >= 'a')
					buffer[i] += 'A' - 'a';
			}
			printf("receive from [%s]:%s.\n", inet_ntoa(their_addr -> sin_addr), buffer);
			write(*new_fd, buffer, strlen(buffer));
		}
		else if (recv_flag == EXIT)
		{
			char time_string[TIME_LENGTH];
			get_time(time_string);

			fprintf(fp, "[%s] is exitting at %s", inet_ntoa(their_addr -> sin_addr), time_string);
			fflush(fp);
			printf("[%s] is exitting at %s", inet_ntoa(their_addr -> sin_addr), time_string);

			close(*new_fd);
			break;
		}
	}

}

int main()
{
	struct sockaddr_in my_addr, their_addr;
	int port;
	int thread_num;
	FILE *fp;
	int sin_size;
	int retval;
	int new_fd[MAX_THREAD];
	fd_set rdfs;
	pthread_t thread[MAX_THREAD];

	fp = fopen("information_server", "r");
	
	fscanf(fp, "port: %d thread: %d", &port, &thread_num);

	printf("The listening port is %d\nThe max thread is %d\n", port, thread_num);


	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(port);
	my_addr.sin_addr.s_addr = INADDR_ANY;

	bzero(&(my_addr.sin_zero), 8);

	if (bind(sockfd, (struct sockaddr*)&my_addr, sizeof (struct sockaddr)) != 0)
		err(1, "WRONG WHEN BIND.");

	listen(sockfd, BACKLOG);


	int i;
	for (i = 0; i < thread_num; ++ i)
	{
		new_fd[i] = accept(sockfd, (struct sockaddr *)&their_addr, (socklen_t *)&sin_size);
		printf("new_fd %d.\n", new_fd[i]);
		void* input[2] = {&new_fd[i], &their_addr};
		if (pthread_create(&thread[i], NULL, (void *)rec, (void *)&input) == -1)
			err(1, "THREAD_CREATE ERROR");

		printf("CREATE THREAD SUCCESS.\n");
	}



	return 0;
}
