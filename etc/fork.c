#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main( )
{
	int pid;

	pid = fork();

	if(pid > 0)
	{
		printf("I'm parent...\n");
		while(1);
	}
	else if(pid == 0)
	{
		printf("I'm child...\n");
		while(1);
	}
	else if(pid == -1)
	{
		perror("fork: ");
		exit(0);
	}
	return 0;
}
