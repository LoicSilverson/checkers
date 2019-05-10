#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char* argv[])
{
	int port_dest = 0;
	int port_this = 0;

	if(argc < 3)
	{
		printf("Did not recieve two port numbers");
		return 0;
	}
	port_dest = atoi(argv[1]);
	if(port_dest == 0)
	{
		printf("Not a proper port number: %s\n", argv[1]);
		return  0;
	}
	port_this = atoi(argv[2]);
	if(port_this == 0)
	{
		printf("Not a proper port number: %s\n", argv[2]);
		return 0;
	}

	printf("Destination port: %d\n", port_dest);
	printf("My port: %d\n", port_this);
}
