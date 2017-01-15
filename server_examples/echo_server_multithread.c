#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void *t_function(void *data)
{
	printf("thread created..\n");
	char buf[1024]={0,};
	int sockfd = *(int*)data;

	read(sockfd,buf,sizeof buf);
	printf("Client said : %s\n",buf);
	write(sockfd,buf,sizeof buf);

	close(sockfd);

	return;
}

int main( )
{
	int size_client_addr, state, i;
	int server_sockfd, client_sockfd[1024];
	struct sockaddr_in server_addr, client_addr;

	pthread_t threads[1024];
	int cnt_threads=0;
	
	server_sockfd = socket(PF_INET, SOCK_STREAM, 0);

	if(server_sockfd < 0)
	{
		perror("socket error: ");
		exit(0);
	}

	memset(&server_addr,0,sizeof server_addr);

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(4000);
	// what is INADDR_ANY and.. what is htons, htonl?

	state = bind(server_sockfd,(struct sockaddr *)&server_addr, sizeof server_addr);
	// what is bind?

	if(state == -1)
	{
		perror("bind error: ");
		exit(0);
	}

	state = listen(server_sockfd,5);

	if(state == -1)
	{
		perror("listen error: ");
		exit(0);
	}

	signal(SIGCHLD,SIG_IGN);

	while(cnt_threads<1024)
	{
		client_sockfd[cnt_threads] = accept(server_sockfd,(struct sockaddr *)&client_addr,&size_client_addr);
		if(client_sockfd[cnt_threads] == -1)
		{
			perror("accept error: ");
			exit(0);
		}

		pthread_create(&threads[cnt_threads], NULL,t_function,(void *)&client_sockfd[cnt_threads]);
		pthread_detach(threads[cnt_threads]);
		cnt_threads++;
	}
	close(server_sockfd);
	return 0;
}