#pragma once

#include <vector>
#include <stdio.h>
#include <mutex>
#include <array>
#include <utility>

#define EMPTY       0x0
#define COLOR_MASK  0x3
#define BLACK       0x1
#define WHITE       0x3
#define TEAM_MASK   0x3
#define QUEEN       0x4

#define CURSOR      0x8

#define UL 0 //UP LEFT
#define UR 1 //UP RIGHT
#define DL 2 //DOWN LEFT
#define DR 3 //DOWN RIGHT
#define END 0xff//END MOVE

#define MOVE_SUCCESS 0
#define MOVE_WRONG_TEAM 1
#define MOVE_ILLEGAL 2

struct position
{
   char row;
   char element;

   void move(char dir)
   {
      if(dir == -1) return;

      char cond_row = row & 0x1;
      char cond_dir = dir & 0x1;

      if(!(cond_row ^ cond_dir)) cond_row ? element++ : element--;

      if(dir & 0x2) row++;
      else row--;
      if(row >= 8 || element >= 4)
      {
         row = -1;
         element = -1;
      }
   }
};


class GameState
{
public:
   char board[8][4];
   bool black_move;
   std::array<std::size_t, 2> players;
   std::mutex mtx;

   bool players_assigned();
   char id_to_team(std::size_t id);

   GameState();
   ~GameState();
   void init_row(char row[], char man);
   void init_board();
   char * tile(const position & pos);
   std::pair<char, bool> register_player(std::size_t id);
   bool unregister_player(std::size_t id);
   char move(std::size_t id, position pos, const std::vector<char>& moves);

};
