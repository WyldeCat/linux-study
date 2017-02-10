#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define EPOLL_SIZE 2048

void handle_err(int state, const char *msg)
{
	if(state<0)
	{
		perror(msg);
		exit(1);
	}
}

int main( )
{
	int server_sockfd, client_sockfd, epoll_fd, state, n, i;
	int readn, client_len;
	char buf[64]={0,};
	struct sockaddr_in server_addr, client_addr;
	struct epoll_event ev, *events;

	// epoll instance 생성
	epoll_fd = epoll_create(5);
	handle_err(epoll_fd, "epoll_create err");

	// server 소켓 생성
	server_sockfd = socket(PF_INET, SOCK_STREAM, 0);
	handle_err(server_sockfd, "socket err");

	memset(&server_addr, 0, sizeof server_addr);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(4000);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	state = bind(server_sockfd, (struct sockaddr*)&server_addr,sizeof server_addr);
	handle_err(state, "bind err");
	state = listen(server_sockfd, 5);
	handle_err(state, "listen err");

	// server 소켓 등록

	ev.events = EPOLLIN;
	ev.data.fd = server_sockfd;

	epoll_ctl(epoll_fd,EPOLL_CTL_ADD,server_sockfd, &ev);

	// 
	events = (struct epoll_event*) malloc(sizeof(*events)*EPOLL_SIZE);

	while(1)
	{
		n = epoll_wait(epoll_fd, events, EPOLL_SIZE, -1);
		// int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
		handle_err(n, "epoll_wait err");

		for(i=0;i<n;i++)
		{
			if(events[i].data.fd == server_sockfd)
			{
				client_sockfd = accept(server_sockfd,(struct sockaddr*)&client_addr,&client_len);	
				handle_err(client_sockfd, "accept err");

				ev.events = EPOLLIN;
				ev.data.fd = client_sockfd;
				epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_sockfd, &ev);
			}
			else 
			{
				readn = read(events[i].data.fd,buf,sizeof buf);
				if(readn == 0)
				{
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_sockfd, NULL);
					continue;
				}
				handle_err(readn,"read err");
				write(events[i].data.fd,buf,readn);
			}
		}
	}
	return 0;
}
