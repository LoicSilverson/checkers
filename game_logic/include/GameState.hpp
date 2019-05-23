#pragma once

#include <vector>
#include <stdio.h>

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
  bool black_move = true;

  GameState();
  ~GameState();
  void init_row(char row[], char man);
  void init_board();
  char * tile(const position & pos);
  void move(position pos, const std::vector<char>& moves);

};
