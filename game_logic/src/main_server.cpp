#include <GameState.hpp>
#include <UDPSocket.hpp>
#include <NetCommon.hpp>

#include <thread>
#include <unordered_map>

#include <stdio.h>
#include <stdlib.h>

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


GameState game;

void print_board(const GameState& g)
{
  auto & board = g.board;

  printf("X |0|0|1|1|2|2|3|3|\n");
  printf("__|_|_|_|_|_|_|_|_|\n");

  for(int row = 0; row < 8; row++)
  if(row %2 == 0)
  //printf("%c %c %c %c \n",
  printf("%d |%d| |%d| |%d| |%d| |\n", row,
  board[row][0],
  board[row][1],
  board[row][2],
  board[row][3]);
  else
  //printf(" %c %c %c %c\n",
  printf("%d | |%d| |%d| |%d| |%d|\n", row,
  board[row][0],
  board[row][1],
  board[row][2],
  board[row][3]);
  printf("Moving team: %s\n", g.black_move ? "BLACK" : "WHITE");
}

struct sockaddr_map : sockaddr_in
{
  bool operator==(const sockaddr_map& that)
  {
    return sin_addr.s_addr == that.sin_addr.s_addr && sin_port == that.sin_port;
  }
  sockaddr_map(const sockaddr_in & that)
  {
    memcpy(this, &that, sizeof(that));
  }
};

struct sockaddrHasher
{
   std::size_t operator()(const sockaddr_map& s) const
   {
      return (size_t)(s.sin_addr.s_addr ^ s.sin_port);
   }
};

bool operator==(const sockaddr_map& first, const sockaddr_map& second)
{
  return first.sin_addr.s_addr == second.sin_addr.s_addr && first.sin_port == second.sin_port;
}


UDPSocket sock;
Buffer msg_in;
Buffer msg_out;
sockaddr_in other;

std::unordered_map<sockaddr_map, bool, sockaddrHasher> clients;

void listen()
{
   printf("Listening\n");
   while(true)
   {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));

      int recv_len = sock.receive(msg_in, (sockaddr*)&other);
      if(recv_len == -1) continue;
      if(ntohs(other.sin_port) == 0) continue;

      int code = msg_in.read<int>();
      if(code == REGISTER)
      {
         printf("Registarion attempt\n");
         clients[(sockaddr_map)other] = true;
         msg_out.write(CONFIRM);
         sock.send(msg_out, (sockaddr*)&other);

         print_board(game);
      }
      else if(code == MSG)
      {
         printf("%d: %s\n", other.sin_port, msg_in.read_c_string());
         msg_out.write_c_string("Acknowledge");
         sock.send(msg_out, (sockaddr*)&other);
      }
      else if(code == MOVE)
      {
        printf("Recieved a move\n");
        position pos{msg_in.read<char>(), msg_in.read<char>()};
        char dir = {msg_in.read<char>()};
        game.move(pos, {dir});
        print_board(game);
      }

      msg_in.reset();
      msg_out.reset();
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

  sock.init();
  sock.bind_port(port);
  listen();
}
