#include <GameState.hpp>
#include <UDPSocket.hpp>
#include <NetCommon.hpp>

#include <thread>
#include <mutex>

#include <stdio.h>
#include <stdlib.h>

#include <BufferHolder.hpp>

UDPSocket sock;
Buffer<512> msg_in;
Buffer<512> msg_out;
sockaddr_in other;
sockaddr_in server_addr;
std::mutex sock_mtx;

BufferHolder<2, 512> holder;
const char dead_frames_limit = 10;
char dead_frames = 0;
std::thread keep_alive_thread;
std::thread user_input_thread;

void keep_alive()
{
   Buffer<sizeof(int)> keep_alive_buff;
   keep_alive_buff.write(int(KEEP_ALIVE));

   while(dead_frames < dead_frames_limit)
   {
      {
         std::lock_guard<std::mutex> lock(sock_mtx);
         sock.send(keep_alive_buff, (sockaddr*)&server_addr);
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(100));
   }
}

void send_move(position pos, char dir)
{
   static Buffer<512> msg_move;
   printf("Moves: %d, %d, %d\n", (int)pos.row, (int)pos.element, (int)dir);
   msg_move.reset();
   msg_move.write(int(MOVE));
   msg_move.write(pos.row);
   msg_move.write(pos.element);
   msg_move.write(dir);

   sock_mtx.lock();
   printf("Sending a move\n");
   sock.send(msg_move, (sockaddr*)&server_addr);
   sock_mtx.unlock();
}

bool reg_server(const char * ip, int port)
{
   static Buffer<sizeof(int)> msg_reg;

   server_addr.sin_addr.s_addr = inet_addr(ip);
   server_addr.sin_port = htons(port);
   server_addr.sin_family = AF_INET;

   sock.set_timeout(1, 0);

   msg_reg.reset();
   msg_reg.write(int(REGISTER));

   sock_mtx.lock();
   sock.send(msg_reg, (sockaddr*)&server_addr);
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
         printf("%d %d %d %d \n",
            board[row][0],
            board[row][1],
            board[row][2],
            board[row][3]);
      else
         printf(" %d %d %d %d\n",
            board[row][0],
            board[row][1],
            board[row][2],
            board[row][3]);
}

void listen()
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

      int code = msg_in.read<int>();

      if(code = KEEP_ALIVE) continue;

      printf("Recieved from %d: %s\n", other.sin_port, msg_in.read_c_string());
      msg_in.reset();
   }
}

void user_input()
{
   printf("Starting the input loop\n");
   printf("To move input: [Position_row Position_column Move]\n");
   printf("Moves are: 0 - Up-Left, 1 - Up-Right, 2 - Down-Left, 3 - Down-Right\n");
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

   keep_alive_thread = std::thread(&keep_alive);
   user_input_thread = std::thread(&user_input);
   listen();
   keep_alive_thread.join();
   user_input_thread.join();
}
