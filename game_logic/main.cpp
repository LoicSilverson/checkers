#include <stdio.h>
#include <stdlib.h>

#include <vector>

#include <termios.h>
#include <stdio.h>

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

#define EMPTY       0x0
#define COLOR_MASK  0x3
#define BLACK       0x1
#define WHITE       0x3
#define TEAM_MASK   0x3
#define QUEEN       0x4

#define CURSOR      0x8

#define UL 0 //UP LEFT
#define UR 1 //UP LEFT
#define DL 2 //UP RIGHT
#define DR 3 //DOWN LEFT
#define END 0xff//END MOVE

struct position
{
   char row;
   char element;
};

void move_position(position & pos, char dir)
{
  if(dir == -1) return;

  char cond_row = pos.row & 0x1;
  char cond_dir = dir & 0x1;

  if(!(cond_row ^ cond_dir)) cond_row ? pos.element++ : pos.element--;

  if(dir & 0x2) pos.row++;
  else pos.row--;
  if(pos.row >= 8 || pos.element >= 4)
  {
    pos.row = -1;
    pos.element = -1;
  }
}

class game_state
{
public:
  char board[8][4];

  void init_row(char row[], char man)
  {
  	int i = 0;
  	for(i = 0; i < 4; i++)
  		row[i] = man;
  }

  void init_board()
  {
  	int i = 0;
  	for(i = 0; i < 3; i++)
  	{
  		init_row(board[i],   BLACK);
  		init_row(board[7-i], WHITE);
  	}
    init_row(board[3], EMPTY);
    init_row(board[4], EMPTY);
  }

  char & tile(const position & pos)
  {
    return board[pos.row][pos.element];
  }

  void move(position pos, const std::vector<char>& moves)
  {
    char &tile_origin = tile(pos);
    if(tile_origin & TEAM_MASK == EMPTY)
    {
      printf("Can not move: the tile is empty\n");
      return;
    }

    auto pos_new = pos;
    move_position(pos_new, moves[0]);
    if(pos_new.row == -1 || pos_new.element == -1)
    {
      printf("Can not move: target tile is outside the board\n");
      return;
    }

    char &tile_target = tile(pos_new);

    if(tile_target & TEAM_MASK != EMPTY)
    {
      printf("Can not move: target tile is occupied\n");
      return;
    }
    tile_target = tile_origin;
    tile_origin = EMPTY;

  }

  game_state()
  {
    printf("Starting the game\n");
    init_board();
  }

  ~game_state(){}

};


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

void print_board(const game_state& g)
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
	int port_dest = 0;
	int port_this = 0;

	if(argc < 3)
	{
		printf("Did not recieve two port numbers\n");
		return 0;
	}
	port_dest = atoi(argv[1]);
	if(port_dest == 0)
	{
		printf("Not a proper port number: %s\n", argv[1]);
		return  0;
	}
	port_this = atoi(argv[2]);
	if(port_this == 0)
	{
		printf("Not a proper port number: %s\n", argv[2]);
		return 0;
	}

	printf("Destination port: %d\n", port_dest);
	printf("My port: %d\n", port_this);
	game_state game;
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
