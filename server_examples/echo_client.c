#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main( )
{
	char buf[1024]={0,};
	int client_sockfd, state, size;
	struct sockaddr_in server_addr;

	client_sockfd = socket(PF_INET, SOCK_STREAM, 0);

	if(client_sockfd < 0)
	{
		perror("socket error: ");
		exit(0);
	}

	memset(&server_addr, 0, sizeof server_addr);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(4000);

	inet_pton(AF_INET,"127.0.0.1",&server_addr.sin_addr.s_addr);

	state = connect(client_sockfd,(struct sockaddr *)&server_addr,sizeof server_addr);

	if(state < 0)
	{
		perror("connect error: ");
		exit(0);
	}

	while(1)
	{
		getchar();
		write(client_sockfd,"hello, world", strlen("hello, world"));
		read(client_sockfd,buf,1024);

		printf("server said... : %s\n",buf);
	}
	close(client_sockfd);
	return 0;
}
