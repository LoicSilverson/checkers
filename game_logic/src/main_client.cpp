#include <GameState.hpp>
#include <UDPSocket.hpp>
#include <NetCommon.hpp>

#include <thread>
#include <mutex>

#include <stdio.h>
#include <stdlib.h>

UDPSocket sock;
Buffer msg_in;
Buffer msg_out;
sockaddr_in other;
sockaddr_in server_addr;
std::mutex sock_mtx;

void send()
{
   msg_out.reset();
   msg_out.write(int(MSG));
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

void send_move(position pos, char dir)
{
  printf("Sending a move\n");
  printf("Moves: %d, %d, %d\n", pos.row, pos.element, dir);
  msg_out.reset();
  msg_out.write(int(MOVE));
  msg_out.write(pos.row);
  msg_out.write(pos.element);
  msg_out.write(dir);

  sock_mtx.lock();
  sock.send(msg_out, (sockaddr*)&server_addr);
  sock_mtx.unlock();
}

bool reg_server(const char * ip, int port)
{
   server_addr.sin_addr.s_addr = inet_addr(ip);
   server_addr.sin_port = htons(port);
   server_addr.sin_family = AF_INET;

   sock.set_timeout(1, 0);

   msg_out.reset();
   msg_out.write(int(REGISTER));

   sock_mtx.lock();
   sock.send(msg_out, (sockaddr*)&server_addr);
   int recv_len = sock.receive(msg_in, (sockaddr*)&other);
   sock_mtx.unlock();

   if(recv_len == -1)
   {
      printf("Did not recieve an answer, could not register\n");
      return false;
   }

   int code = msg_in.read<int>();
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

  printf("Starting the input loop\n");
  printf("To move input: [Position_row Position_column Move]\n");
  printf("Moves are: 0 - Up-Left, 1 - Up-Right, 3 - Down-Left, 4 - Down-Right\n");
  int input;
  position pos;
  char dir;
  while(true)
  {
    scanf("%d", &input);
    pos.row = input;
    scanf("%d", &input);
    pos.element = input;
    scanf("%d", &input);
    dir = input;
    send_move(pos, dir);
  };

   //send();
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
