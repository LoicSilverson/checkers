#include <UDPSocket.hpp>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

int UDPSocket::receive(Buffer& buf, struct sockaddr * source)
{
  unsigned int source_len;
  return recvfrom(sock_id, buf.m_data, BUFLEN, 1, source, &source_len);
}

int UDPSocket::send(const Buffer& buf, struct sockaddr* target)
{
  return sendto(sock_id, buf.m_data, buf.m_size, 0, target, sizeof(*target));
}
