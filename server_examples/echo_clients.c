#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

int main(int argc,char *argv[])
{
	freopen(argv[1],"w",stdout);
	signal(SIGCHLD, SIG_IGN);
	fork();
	fork();
	fork();
	fork();
	fork();
	fork();
	fork();
	fork();
	fork();
	fork();
	
	char buf[]="hello, world";
	int client_sockfd, size, i, state;
	uint64_t delta_us=0;

	struct sockaddr_in server_addr;
	struct timespec start, end;

	client_sockfd = socket(PF_INET, SOCK_STREAM, 0);
	memset(&server_addr,0,sizeof server_addr);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(4000);

	inet_pton(AF_INET,"192.168.1.208",&server_addr.sin_addr.s_addr);

	state = connect(client_sockfd,(struct sockaddr *)&server_addr, sizeof server_addr);
	
	if(state<0)
	{
		perror("connect err ");
		exit(1);
	}

	for(i=0;i<10;i++)
	{
		int n;
		clock_gettime(CLOCK_MONOTONIC_RAW, &start);
		n=write(client_sockfd,buf,sizeof buf);
		if(n<0)
		{
			perror("write err ");
			exit(1);
		}
		n=read(client_sockfd,buf,64);
		if(n<0)
		{
			perror("read err ");
			exit(1);
		}
		clock_gettime(CLOCK_MONOTONIC_RAW, &end);

		delta_us += (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec)/1000;
		sleep(1);
	}
	printf("%lu\n",delta_us);

	return 0;
}
