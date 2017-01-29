#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

void* t(void* data)
{
	while(1)
	{
		fprintf(stderr,"!");
		sleep(1);
	}
}

int main( )
{
	char buf[64]={0,};

	pthread_t a;
	pthread_create(&a,NULL,t,NULL);

	while(1)
	{
		scanf("%s",buf);
		printf("%s\n",buf);
	}

	return 0;
}
