#include <GameState.hpp>
#include <UDPSocket.hpp>

#include <stdio.h>
#include <stdlib.h>

#include <thread>

#include <termios.h>

static termios _old, _new;

/* Initialize new terminal i/o settings */
void initTermios(int echo)
{
  tcgetattr(0, &_old); /* grab _old terminal i/o settings */
  _new = _old; /* make _new settings same as _old settings */
  _new.c_lflag &= ~ICANON; /* disable buffered i/o */
  if (echo) {
      _new.c_lflag |= ECHO; /* set echo mode */
  } else {
      _new.c_lflag &= ~ECHO; /* set no echo mode */
  }
  tcsetattr(0, TCSANOW, &_new); /* use these _new terminal i/o settings now */
}

/* Restore _old terminal i/o settings */
void resetTermios(void)
{
  tcsetattr(0, TCSANOW, &_old);
}

/* Read 1 character - echo defines echo mode */
char getch_(int echo)
{
  char ch;
  initTermios(echo);
  ch = getchar();
  resetTermios();
  return ch;
}
/* Read 1 character without echo */
char getch(void)
{
  return getch_(0);
}

/* Read 1 character with echo */
char getche(void)
{
  return getch_(1);
}

UDPSocket sock;
Buffer msg_in;
Buffer msg_out;
sockaddr_in other;

void listen()
{
   printf("Listening\n");
   msg_out.write_c_string("Acknowledged");
   while(true)
   {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));

      int recv_len = sock.receive(msg_in, (sockaddr*)&other);
      if(recv_len == -1) continue;
      if(ntohs(other.sin_port) == 0) continue;
      printf("Recieved from %d: %s\n", other.sin_port, msg_in.read_c_string());
      sock.send(msg_out, (sockaddr*)&other);

      msg_in.reset();
   }
}

int main(int argc, const char* argv[])
{
	int port = 0;

	if(argc < 2)
	{
		printf("Did not recieve port number\n");
		return 0;
	}
	port = atoi(argv[1]);
	if(port == 0)
	{
		printf("Not a proper port number: %s\n", argv[1]);
		return  0;
	}

	printf("My port: %d\n", port);
	GameState game;
   sock.init();
   sock.bind_port(port);
   listen();
}
