#include <iostream>

int main(int argc, const char* argv[])
{
	int port_dest = 0;
	int port_this = 0;
	
	if(argc < 3)
	{
		std::cout << "Did not recieve two port numbers";
		return 0;
	}
	port_dest = std::atoi(argv[1]);
	if(port_dest == 0)
	{
		std::cout << "Not a proper port number: " << argv[1] << "\n";
		return  0;
	}
	port_this = std::atoi(argv[2]);
	if(port_this == 0)
	{
		std::cout << "Not a proper port number: " << argv[2] << "\n";
		return 0;
	}

	std::cout << "Destination port: " << port_dest << "\n";
	std::cout << "My port: " << port_this << "\n";
}
