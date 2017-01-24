#include<sys/epoll.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

#define EPOLL_SIZE 2048 

void handle_err(int state, char *msg) 
{
	if(state<0)
	{
		perror(msg);
		exit(1);
	}
}

int main( )
{
	int efd,i,n,readn, count=0;
	int server_sockfd, client_sockfd, state, client_len;
	char buf[64]={0,};
	struct sockaddr_in server_addr, client_addr;
	struct epoll_event ev, *events;
	
	// 1. epoll 인스턴스 생성
	efd = epoll_create(5);

	handle_err(efd,"epoll_create err");

	// 2. socket 생성
	server_sockfd = socket(PF_INET, SOCK_STREAM, 0);
	memset(&server_addr,0,sizeof server_addr);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(4000);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	state = bind(server_sockfd,(struct sockaddr*)&server_addr,sizeof server_addr);
	handle_err(state,"bind err");
	state = listen(server_sockfd,5);

	// 1, 2, 4, 8... 001 compose 가능
	ev.events = EPOLLIN;
	ev.data.fd = server_sockfd;

	epoll_ctl(efd,EPOLL_CTL_ADD,server_sockfd, &ev);
	events = (struct epoll_event*)malloc(sizeof(*events)*EPOLL_SIZE);

	while(1)
	{
		n = epoll_wait(efd, events, EPOLL_SIZE, -1);	
		for(i=0;i<n;i++)
		{
			if(events[i].data.fd == server_sockfd)
			{
				client_sockfd = accept(server_sockfd, (struct sockaddr*)&client_addr,&client_len);	
				fprintf(stderr,"Client accpeted.. %d %d\n",client_sockfd,++count);
				ev.events = EPOLLIN;
				ev.data.fd = client_sockfd;
				epoll_ctl(efd, EPOLL_CTL_ADD, client_sockfd, &ev);
			}
			else  // 클라이언트 소켓이.. read가 가능함
			{
				readn = read(events[i].data.fd, buf, sizeof buf);		
				buf[readn]=0;
				handle_err(readn,"read err");
				if(readn == 0)
				{
					epoll_ctl(efd, EPOLL_CTL_DEL, events[i].data.fd, NULL);	
					close(events[i].data.fd);
					continue;
				}
				// echo
				state = write(events[i].data.fd, buf, readn);
				handle_err(state,"write err");
			}
		}
	}
	return 0;
}
