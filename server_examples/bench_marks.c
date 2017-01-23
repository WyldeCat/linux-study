#include<stdio.h>
#include<stdlib.h>

int main(int argc,char *argv[])
{
	if(argc==0)
	{
		printf("error\n");
		exit(1);
	}

	freopen(argv[1],"r",stdin);
	unsigned long long sum=0;
	unsigned long long count=0;
	int n;

	while(scanf("%d",&n)!=EOF) 
	{
		sum+=n;
		count++;
	}

	printf("Total requests : %llu\n",count);
	printf("Total response time : %llu\n",sum);
	printf("Average response time: %lf\n",sum/(double)count);

	return 0;
}
