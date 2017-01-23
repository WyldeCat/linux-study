#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define EPOLL_SIZE 2048
#define EPOLL_CNT 3

int efd[EPOLL_CNT+1];
struct epoll_event ev[EPOLL_CNT+1], *events[EPOLL_CNT+1];

void *t_function(void *data)
{
	int idx = *((int*)data);
	int epoll_fd = efd[idx], i, n, readn;
	char buf[64];

	struct epoll_event *event = events[idx];

	while(1)
	{
		n = epoll_wait(epoll_fd, event, EPOLL_SIZE, 0);
		for(i=0;i<n;i++)
		{
			readn = read(event[i].data.fd,buf,sizeof buf);
			fprintf(stderr,"%d  %d %s\n",event[i].data.fd, readn, buf);
			if(readn < 0)
			{
				perror("read error ");
				exit(1);
			}
			if(readn == 0)
			{
				epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event[i].data.fd, event);
				continue;
			}

			buf[readn]=0;
			write(event[i].data.fd, buf, readn);
		}
	}
}

int main( )
{
	int server_sockfd, client_sockfd, client_len, i, count=0, n, state;
	int idices[EPOLL_CNT];
	struct sockaddr_in server_addr, client_addr;
	
	pthread_t threads[EPOLL_CNT];

	server_sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if(socket<0)
	{
		perror("socket error");
		exit(1);
	}

	memset(&server_addr,0,sizeof server_addr);

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(4000);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	state = bind(server_sockfd,(struct sockaddr*)&server_addr,sizeof server_addr);	
	if(state<0)
	{
		perror("bind error");
		exit(1);
	}
	state = listen(server_sockfd,5);
	if(state<0)
	{
		perror("listen error");
		exit(1);
	}
	

	for(i=0;i<=EPOLL_CNT;i++)
	{
		idices[i]=i;
		events[i] = (struct epoll_event*)malloc(sizeof(*events[i]) * EPOLL_SIZE);
		efd[i] = epoll_create(EPOLL_SIZE);
		if(i==EPOLL_CNT) continue;
		// EPOLL_CNT is with main thread
		pthread_create(&threads[i],NULL,t_function,(void*)&idices[i]);
	}
	
	ev[EPOLL_CNT].events = EPOLLIN;
	ev[EPOLL_CNT].data.fd = server_sockfd;
	epoll_ctl(efd[EPOLL_CNT], EPOLL_CTL_ADD, server_sockfd, &ev[EPOLL_CNT]);

	while(1)
	{
		n = epoll_wait(efd[EPOLL_CNT], events[EPOLL_CNT], 1, 0);
		for(i=0;i<n;i++)
		{
			if(events[EPOLL_CNT][i].data.fd == server_sockfd)
			{
				int idx = count%EPOLL_CNT;
				client_sockfd = accept(server_sockfd, (struct sockaddr*)&client_addr, &client_len);
				if(client_sockfd <0)
				{
					perror("accept error");
					exit(1);
				}
				ev[EPOLL_CNT].events = EPOLLIN;
				ev[EPOLL_CNT].data.fd = client_sockfd;
				epoll_ctl(efd[idx], EPOLL_CTL_ADD, client_sockfd, &ev[EPOLL_CNT]);
				count++;
				if(count%100==0)
				{
					printf("%d\n",count);
				}
			}
		}
	}

	return 0;
}
