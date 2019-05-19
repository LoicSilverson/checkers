#pragma once

#include <Buffer.hpp>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

class UDPSocket
{
  int sock_id;
  sockaddr addr;
public:
  int init();
  int set_timeout(int seconds, int nanoseconds);
  int bind_port(int port);
  int receive(Buffer& buf, struct sockaddr * source);
  int send(const Buffer& buf, struct sockaddr* target);
};
