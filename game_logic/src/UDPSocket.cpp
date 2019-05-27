#include <UDPSocket.hpp>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

bool sockaddr_key::operator==(const sockaddr_key& that)
{
   return sin_addr.s_addr == that.sin_addr.s_addr && sin_port == that.sin_port;
}
sockaddr_key::sockaddr_key(const sockaddr_in & that)
{
   memcpy(this, &that, sizeof(that));
}

std::size_t sockaddrHasher::operator()(const sockaddr_key& s) const
{
   return (size_t)(s.sin_addr.s_addr ^ s.sin_port);
}

bool operator==(const sockaddr_key& first, const sockaddr_key& second)
{
  return first.sin_addr.s_addr == second.sin_addr.s_addr && first.sin_port == second.sin_port;
}

int UDPSocket::init()
{
  printf("Start init\n");
  if ((sock_id = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    return -1;

  struct timeval read_timeout;
  read_timeout.tv_sec = 0;
  read_timeout.tv_usec = 10;

  if (setsockopt(sock_id, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof(struct timeval)) < 0)
  {
    return -2;
  }
  printf("End init\n");
  return 1;
}

int UDPSocket::set_timeout(int seconds, int nanoseconds)
{
  struct timeval read_timeout;
  read_timeout.tv_sec = seconds;
  read_timeout.tv_usec = nanoseconds;

  if (setsockopt(sock_id, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof(struct timeval)) < 0)
  {
    return -2;
  }
  return 1;
}

int UDPSocket::bind_port(int port)
{
  int enable = 1;
  if (setsockopt(sock_id, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    return -1;

  sockaddr_in addr;
  memset((char *)&addr, 0, sizeof(addr));

  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);

  if( bind(sock_id, (sockaddr*)&addr, sizeof(addr) ) == -1)
    return -1;
  return 1;
}
