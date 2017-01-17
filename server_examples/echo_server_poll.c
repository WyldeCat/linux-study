#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <poll.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void handle_error(int state, char *step)
{
	if(state == -1) 
	{
		fprintf(stderr,"%s error.. \n",step);
		exit(1);
	}
}

int main( )
{
	int server_sockfd, client_sockfd, state, i, nfds=1;
	int client_len;
	char buf[1024]={0,};

	struct sockaddr_in server_addr, client_addr;
	struct pollfd client[1024];
	
	server_sockfd = socket(PF_INET, SOCK_STREAM, 0);

	memset(&server_addr, 0, sizeof server_addr);
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(4000);

	state = bind(server_sockfd, (struct sockaddr*)&server_addr, sizeof server_addr);	
	handle_error(state, "bind");

	state = listen(server_sockfd, 5);
	handle_error(state, "listen");

	client[0].fd = server_sockfd;	
	client[0].events = POLLIN;
	client[0].revents = 0;

	for(i=1; i<1024; i++)
	{
		client[i].fd = -1;
		client[i].events = POLLIN;
		client[i].revents = 0;
	}

	while(1)
	{
		int state = poll(client, nfds, 1000);	
		handle_error(state, "poll");

		if(client[0].revents & POLLIN)
		{
			fprintf(stderr,"Accepted..\n");

			client_sockfd = accept(server_sockfd, (struct sockaddr*)&client_addr, &client_len);	
			for(i=0;i<nfds;i++)
			{
				if(client[i].fd == -1)
				{
					client[i].fd = client_sockfd;
					break;
				}
			}
			if(i==nfds)
			{
				client[nfds++].fd = client_sockfd;
			}
		}

		for(i=1;i<nfds;i++)
		{
			if(client[i].revents & (POLLIN|POLLERR))
			{
				int nread = read(client[i].fd, buf, 1024);
				buf[nread]=0;
				if(nread==0)
				{
					client[i].fd=-1;
					continue;
				}
				fprintf(stderr,"%s Received... I'll echo that.\n",buf);
				// bug?
				write(client[i].fd, buf, nread);
			}
		}
	}
}
