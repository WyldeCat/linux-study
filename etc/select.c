#include <sys/time.h>
#include <sys/select.h>

#include <fcntl.h>
// What for?
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main( )
{
	int fd[2];
	int i,n,state;

	char buf[255];

	struct timeval tv;

	fd_set readfds, writefds;

	if((fd[0] = open("/tmp/testfile1",O_RDONLY)) == -1)
	{
		perror("file open error :");
		exit(0);
	}
	if((fd[1] = open("/tmp/testfile2",O_RDONLY)) == -1)
	{
		perror("file open error :");
		exit(0);
	}
	printf("%d %d\n",fd[0],fd[1]);
	memset(buf, 0, sizeof buf);

	for(;;)
	{
		FD_ZERO(&readfds);
		FD_SET(fd[0], &readfds);
		FD_SET(fd[1], &readfds);
		state = select(fd[1]+1, &readfds, NULL, NULL, NULL);
		switch(state)
		{
			case -1:
				perror("select error: ");
				exit(0);
				break;
			
			default:
				for(i = 0; i < 2; i++)
				{
					if(FD_ISSET(fd[i], &readfds))
					{
						printf("I can read %d!!\n",i);
						while((n=read(fd[i], buf, 255)) > 0)
						{ // how read works?
							printf("(%d) [%d] %s\n", state, i, buf);
						}
						memset(buf, 0, sizeof buf);
					}
				}
				break;
		}
		fflush(stdout);
		sleep(1);
	}

	return 0;
}
