#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define max(a,b) (((a) > (b))? (a) : (b))
void handle_error(int state, char *step)
{
	if(state == -1)
	{ 
		fprintf(stderr,"%s error\n",step);
		exit(1);
	}
}

int main( )
{
	int server_sockfd, client_sockfd, state, max_fd, fd_num;
	int i, n, option=1;
	char buf[1024]={0,};
	fd_set read_fds, temp;

	struct sockaddr_in server_addr, client_addr;
	int client_addr_len;

	server_sockfd = socket(PF_INET, SOCK_STREAM, 0);
	handle_error(server_sockfd, "socket");

	setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof option);

	memset(&server_addr, 0, sizeof server_addr);

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(4000);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	state = bind(server_sockfd, (struct sockaddr*)&server_addr, sizeof server_addr);
	handle_error(state, "bind");

	state = listen(server_sockfd, 5);
	handle_error(state, "listen");

	FD_ZERO(&read_fds);
	FD_SET(server_sockfd, &read_fds);

	max_fd = server_sockfd + 1;	

	while(1)
	{
		temp = read_fds;
		fd_num = select(1024, &temp, NULL, NULL, NULL);

		handle_error(fd_num, "select");	

		if(FD_ISSET(server_sockfd,&temp))
		{
			client_sockfd = accept(server_sockfd, (struct sockaddr*)&client_addr,&client_addr_len);	
			handle_error(client_sockfd, "accept");

			FD_SET(client_sockfd, &read_fds);
		}

		for(i=0; i<1024; i++)
		{
			if(i==server_sockfd) continue;

			if(FD_ISSET(i,&temp))
			{
				n = read(i, buf, sizeof buf);	
				if(n==0)
				{
					FD_CLR(i,&read_fds);
				}
				else 
				{
					buf[n]=0;
					write(i, buf, n);
				}
			}
		}
	}
	return 0;
}
