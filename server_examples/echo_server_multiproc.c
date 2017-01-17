#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <signal.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main( )
{
	int server_sockfd, client_sockfd;
	int pid, state, client_len;

	struct sockaddr_in clientaddr, server_addr;
	char buf[1024];

	if((server_sockfd = socket(PF_INET,SOCK_STREAM,0)) <0)
	{
		perror("socket error: ");
		exit(0);
	}

	memset(&server_addr,0,sizeof server_addr);
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(4000);

	state = bind(server_sockfd, (struct sockaddr *)&server_addr, sizeof server_addr);

	if(state == -1)
	{
		perror("bind error: ");
		exit(0);
	}

	state = listen(server_sockfd, 1);

	if(state == -1)
	{
		perror("listen error: ");
		exit(0);
	}

	signal(SIGCHLD, SIG_IGN);
	while(1)
	{
		client_sockfd = accept(server_sockfd, (struct sockaddr*)&clientaddr, &client_len);
		if(client_sockfd == -1)
		{
			perror("Accept error: ");
			exit(0);
		}
		
		printf("Accept success!\n");

		pid = fork();

		if(pid == 0)
		{
			while(1)
			{
				memset(buf,0x00,1024);
				if(read(client_sockfd, buf, 1024) <= 0)
				{
					printf("closing..!\n");
					close(client_sockfd);
					exit(0);
				}
				printf(" > %s\n",buf);
				write(client_sockfd, buf, strlen(buf));
			}
		}
		else if(pid == -1)
		{
			perror("fork error: ");
			return 1;
		}
	}
	return 0;
}
