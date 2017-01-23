#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

void handle_err(int state, char *msg)
{
	if(state < 0)
	{
		fprintf(stderr,"%s\n",msg);
		exit(1);
	}
}

int main( )
{
	int server_sockfd, client_sockfd, client_len;
	int i, readn, state, flag, errno;
	char buf[1024]={0,};
	struct sockaddr_in server_addr, client_addr;

	server_sockfd = socket(PF_INET, SOCK_STREAM, 0);
	handle_err(server_sockfd, "socket error");

	/* set flag nonblocking */
	flag = fcntl(server_sockfd, F_GETFL, 0);
	fcntl(server_sockfd, F_SETFL, flag | O_NONBLOCK);
	

	memset(&server_addr, 0, sizeof server_addr);
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(4000);

	state = bind(server_sockfd, (struct sockaddr*)&server_addr, sizeof server_addr);
	handle_err(state, "bind error");

	state = listen(server_sockfd, 5);
	handle_err(state, "listen error");

	while(1)
	{
		client_sockfd = accept(server_sockfd, (struct sockaddr*)&client_addr, &client_len);
		if(client_sockfd != -1) break;
	}
	fprintf(stderr,"Client connected..\n");

	while(1)
	{
		fprintf(stderr,"Waiting..\n");
		readn = read(client_sockfd, buf, 1024);
		fprintf(stderr,"I received!!\n");

		if(readn == 0)
		{
			close(client_sockfd);
			close(server_sockfd);
			break;
		}
		else if(readn == -1)
		{
			fprintf(stderr,"But sadly, I think there is no data..\n");
			sleep(1);
		}
		else 
		{
			buf[readn]=0;
			fprintf(stderr,"Client said.. : %s\n", buf);
			state = write(client_sockfd, buf, readn);
		}
		fprintf(stderr,"\n\n");
	}

	return 0;
}
