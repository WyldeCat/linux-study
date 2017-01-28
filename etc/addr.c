#include <arpa/inet.h>
#include <stdio.h>

int main( )
{
	struct sockaddr_in addr;
	char *buf;

	addr.sin_family = PF_INET;
	addr.sin_port = htons(4000);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	buf = inet_ntoa(addr.sin_addr);

	inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr.s_addr);

	buf = inet_ntoa(addr.sin_addr);
	printf("%s\n",buf);
	return 0;
}
