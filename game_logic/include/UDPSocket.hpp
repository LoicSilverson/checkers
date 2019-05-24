#pragma once

#include <Buffer.hpp>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

struct sockaddr_key : sockaddr_in
{
  bool operator==(const sockaddr_key& that);
  sockaddr_key(const sockaddr_in & that);
};

struct sockaddrHasher
{
   size_t operator()(const sockaddr_key& s) const;
};

bool operator==(const sockaddr_key& first, const sockaddr_key& second);

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
