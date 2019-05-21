#include <GameState.hpp>
#include <UDPSocket.hpp>
#include <UDPCodes.hpp>

#include <thread>
#include <mutex>

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


UDPSocket sock;
Buffer msg_in;
Buffer msg_out;
sockaddr_in other;
sockaddr_in server_addr;
std::mutex sock_mtx;

//void listen

void send()
{
   msg_in.reset();
   msg_out.reset();
   msg_out.write_int(MSG);
   msg_out.write_c_string("How are you doing?");
   while(true)
   {
      printf("Sending\n");

      sock_mtx.lock();
      sock.send(msg_out, (sockaddr*)&server_addr);
      sock_mtx.unlock();

      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
   }
}

bool reg_server(const char * ip, int port)
{
   server_addr.sin_addr.s_addr = inet_addr(ip);
   server_addr.sin_port = htons(port);
   server_addr.sin_family = AF_INET;

   sock.set_timeout(1, 0);

   msg_out.reset();
   msg_out.write_int(REGISTER);

   sock_mtx.lock();
   sock.send(msg_out, (sockaddr*)&server_addr);
   int recv_len = sock.receive(msg_in, (sockaddr*)&other);
   sock_mtx.unlock();

   if(recv_len == -1)
   {
      printf("Did not recieve an answer, could not register\n");
      return false;
   }

   int code = msg_in.read_int();
   if(code == CONFIRM)
   {
      printf("Registered successfully\n");
      sock.set_timeout(0, 10);
      return true;
   }

   if(code == DENY)
   {
      printf("Server denied registration, could not register\n");
      return false;
   }

   printf("Resieved trash from server, could not register\n");
   return false;
}


void print_board(const GameState& g)
{
  auto & board = g.board;
  int row;
  for(row = 0; row < 8; row++)
    if(row %2 == 0)
      //printf("%c %c %c %c \n",
      printf("%d %d %d %d \n",
        board[row][0],
        board[row][1],
        board[row][2],
        board[row][3]);
    else
      //printf(" %c %c %c %c\n",
      printf(" %d %d %d %d\n",
        board[row][0],
        board[row][1],
        board[row][2],
        board[row][3]);
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

	GameState game;

   sock.init();
   if(!reg_server("127.0.0.1", port)) return 0;

   std::thread first ([&]()
   {
      printf("Listening\n");
      while(true)
      {
         std::this_thread::sleep_for(std::chrono::milliseconds(100));

         sock_mtx.lock();
         int recv_len = sock.receive(msg_in, (sockaddr*)&other);
         sock_mtx.unlock();
         if(recv_len == -1) continue;
         if(ntohs(other.sin_port) == 0) continue;
         printf("Recieved from %d: %s\n", other.sin_port, msg_in.read_c_string());
         msg_in.reset();
      }
   });

   send();
   /*
   print_board(game);

   game.move({2, 0}, {DR});
   printf("= = = = = = =\n");
   print_board(game);

   game.move({3, 0}, {DL});
   printf("= = = = = = =\n");
   print_board(game);

   game.move({4, 0}, {DL});
   printf("= = = = = = =\n");
   print_board(game);

   game.move({4, 0}, {DR});
   printf("= = = = = = =\n");
   print_board(game);
   */
}
