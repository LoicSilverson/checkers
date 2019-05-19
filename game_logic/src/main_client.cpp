#include <GameState.hpp>

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


//void update(char board[8][4])
//{
//  position pos;
//  pos.row = 0;
//  pos.element = 0;
//  char c;
//  char dir;
//  while(1)
//  {
//    board[pos.row][pos.element] |= CURSOR;
//    game_print_board(board);
//    printf("= = = = = =\n");
//
//    c = getche();
//    printf("\n");
//
//    switch(c)
//    {
//    case 'q': dir = UL;
//      break;
//    case 'w': dir = UR;
//      break;
//    case 'a': dir = DL;
//      break;
//    case 's': dir = DR;
//      break;
//    default: dir = -1;
//    }
//
//    board[pos.row][pos.element] &= ~CURSOR;
//    game_move_position(&pos, dir);
//    if(pos.row == -1) return;
//  }
//}

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

}
