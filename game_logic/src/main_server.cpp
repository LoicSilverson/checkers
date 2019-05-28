#include <GameState.hpp>
#include <UDPSocket.hpp>
#include <NetCommon.hpp>
#include <BufferHolder.hpp>

#include <thread>
#include <unordered_map>

#include <stdio.h>
#include <stdlib.h>

#include <mutex>


#include <vector>

GameState game;
BufferHolder<2, 512> holder;

UDPSocket sock;
Buffer<512> msg_in;
Buffer<512> msg_out;
std::mutex sock_mutex;
sockaddr_in other;

void print_board(const GameState& g)
{
   auto & board = g.board;

   printf("X |0|0|1|1|2|2|3|3|\n");
   printf("__|_|_|_|_|_|_|_|_|\n");

   for(int row = 0; row < 8; row++)
   if(row %2 == 0)
      printf("%d |%d| |%d| |%d| |%d| |\n", row,
         board[row][0],
         board[row][1],
         board[row][2],
         board[row][3]);
   else
      printf("%d | |%d| |%d| |%d| |%d|\n", row,
         board[row][0],
         board[row][1],
         board[row][2],
         board[row][3]);
   printf("Moving team: %s\n", g.black_move ? "BLACK" : "WHITE");
}

struct client
{
   char team;
   char dead_frames;
};

const char dead_frames_limit = 10;

std::unordered_map<sockaddr_key, client, sockaddrHasher> clients;
std::mutex mtx_clients;

bool registration(const sockaddr_key& other)
{
   printf("Registarion attempt\n");
   static Buffer<2*sizeof(int)> msg_reg_resp;
   msg_reg_resp.reset();

   std::pair<char, bool> game_reg;
   {
      std::lock_guard<std::mutex> lock(sock_mutex);
      if(clients.size() >= 2)
      {
         printf("Registarion denied\n");
         msg_reg_resp.write(DENY);
         sock.send(msg_reg_resp, (sockaddr*)&other);
         return false;
      }
      else
      {
         auto id = sockaddrHasher()(other);
         printf("Registarion procedes for id %d\n", id);
         game_reg = game.register_player(id);

         clients[(sockaddr_key)other] = {0, game_reg.first};
         printf("%d\n", game_reg.first);

         msg_reg_resp.write(CONFIRM);
         msg_reg_resp.write(game_reg.first);

         sock.send(msg_reg_resp, (sockaddr*)&other);
      }
   }
   if(game_reg.second)
   {
      game.init_board();
      print_board(game);
   }

}

void listen()
{
   printf("Listening\n");
   while(true)
   {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      {
         std::lock_guard<std::mutex> lock(sock_mutex);
         int recv_len = sock.receive(msg_in, (sockaddr*)&other);
         if(recv_len == -1) continue;
         if(ntohs(other.sin_port) == 0) continue;
      }

      auto x = clients.find(other);
      if(x != clients.end())
         x->second.dead_frames = 0;

      int code = msg_in.read<int>();

      if(code == REGISTER)
      {
         registration(other);
      }
      else if(code == MSG)
      {
         printf("%d: %s\n", other.sin_port, msg_in.read_c_string());
         sock_mutex.lock();
         msg_out.write_c_string("Acknowledge");
         sock.send(msg_out, (sockaddr*)&other);
         sock_mutex.unlock();
      }
      else if(code == MOVE)
      {
         printf("Recieved a move\n");
         position pos{msg_in.read<char>(), msg_in.read<char>()};
         char dir = {msg_in.read<char>()};
         game.move(sockaddrHasher()(other), pos, {dir});
         print_board(game);
      }

      sock_mutex.lock();
      msg_in.reset();
      msg_out.reset();
      sock_mutex.unlock();
   }
}

void keep_alive()
{
   Buffer<sizeof(int)> keep_alive_buff;
   keep_alive_buff.write(int(KEEP_ALIVE));
   while(true)
   {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      {
         std::lock_guard<std::mutex> lock(sock_mutex);
         for(auto client_it = clients.begin(); client_it != clients.end();)
         {
            if(client_it->second.dead_frames >= dead_frames_limit)
            {
               printf("Removing client for inactivity\n");
               game.unregister_player(sockaddrHasher()(client_it->first));
               clients.erase(client_it++);
               continue;
            }
            sock.send(keep_alive_buff, (sockaddr*)&client_it->first);
            client_it->second.dead_frames++;
            ++client_it;
         }
      }
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
   std::thread keep_alive_thread(&keep_alive);
   listen();
   keep_alive_thread.join();
}
