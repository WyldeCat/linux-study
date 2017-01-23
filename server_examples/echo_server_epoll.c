#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#define EPOLL_SIZE 20

void handle_err(int state,char *msg)
{
	if(state == -1)
	{
		fprintf(stderr,"%s\n",msg);
		exit(1);
	}
}

int main( )
{
	struct sockaddr_in server_addr, client_addr;
	int server_sockfd, client_sockfd, client_len;
	int flags = 1, n, i, readn, state;
	struct epoll_event ev, *events;
	int epoll_fd;

	char buf[1024]={0,};

	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	handle_err(server_sockfd, "socket error");	

	server_addr.sin_family = PF_INET;
	server_addr.sin_port = htons(4000);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	state = bind(server_sockfd,(struct sockaddr*)&server_addr,sizeof server_addr);
	handle_err(state,"bind error");
	state = listen(server_sockfd, 5);
	handle_err(state,"listen error");

	events = (struct epoll_event*)malloc(sizeof(*events) * EPOLL_SIZE);

	if((epoll_fd = epoll_create(100))<0)
	{
		perror("epoll_create error");
		return 1;
	}

	ev.events = EPOLLIN;
	ev.data.fd = server_sockfd;
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_sockfd, &ev);


	while(1)
	{
		n = epoll_wait(epoll_fd, events, EPOLL_SIZE, -1);
		handle_err(n,"epoll wait error");

		for(i=0;i<n;i++)
		{
			if(events[i].data.fd == server_sockfd)
			{
				printf("Client accepted..\n");
				client_sockfd = accept(server_sockfd, (struct sockaddr*)&client_addr, &client_len);

				ev.events = EPOLLIN;
				ev.data.fd = client_sockfd;
				epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_sockfd, &ev);
			}
			else 
			{
				memset(buf, 0, sizeof buf);
				readn = read(events[i].data.fd, buf, szeof buf);
				handle_err(readn,"read error");
				if(readn==0)
				{
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, events);
					close(events[i].data.fd);
					printf("connection closed.\n");
					continue;
				}
				printf("read data %s\n", buf);
				write(events[i].data.fd, buf, readn);
			}
		}
	}

	return 0;
}
