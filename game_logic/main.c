#include <stdio.h>
#include <stdlib.h>

#include <termios.h>
#include <stdio.h>

static struct termios old, new;

/* Initialize new terminal i/o settings */
void initTermios(int echo)
{
  tcgetattr(0, &old); /* grab old terminal i/o settings */
  new = old; /* make new settings same as old settings */
  new.c_lflag &= ~ICANON; /* disable buffered i/o */
  if (echo) {
      new.c_lflag |= ECHO; /* set echo mode */
  } else {
      new.c_lflag &= ~ECHO; /* set no echo mode */
  }
  tcsetattr(0, TCSANOW, &new); /* use these new terminal i/o settings now */
}

/* Restore old terminal i/o settings */
void resetTermios(void)
{
  tcsetattr(0, TCSANOW, &old);
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

void game_init_row(char row[], char man)
{
	int i = 0;
	for(i = 0; i < 4; i++)
		row[i] = man;
}

void game_init_board(char board[8][4])
{
	int i = 0;
	for(i = 0; i < 3; i++)
	{
		game_init_row(board[i],   BLACK);
		game_init_row(board[7-i], WHITE);
	}
   game_init_row(board[3], EMPTY);
   game_init_row(board[4], EMPTY);
}

void game_print_board(char board[8][4])
{
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

void game_move_position(struct position * pos, char dir)
{
  if(dir == -1) return;

  char cond_row = pos->row & 0x1;
  char cond_dir = dir & 0x1;

  if(!(cond_row ^ cond_dir)) cond_row ? pos->element++ : pos->element--;

  if(dir & 0x2) pos->row++;
  else pos->row--;
  if(pos->row >= 8 || pos->element >= 4)
  {
    pos->row = -1;
    pos->element = -1;
  }
}

void game_move(char board[8][4], struct position pos, char * moves)
{

}

void game_update(char board[8][4])
{
  struct position pos;
  pos.row = 0;
  pos.element = 0;
  char c;
  char dir;
  while(1)
  {
    board[pos.row][pos.element] |= CURSOR;
    game_print_board(board);
    printf("= = = = = =\n");

    c = getche();
    printf("\n");

    switch(c)
    {
    case 'q': dir = UL;
      break;
    case 'w': dir = UR;
      break;
    case 'a': dir = DL;
      break;
    case 's': dir = DR;
      break;
    default: dir = -1;
    }

    board[pos.row][pos.element] &= ~CURSOR;
    game_move_position(&pos, dir);
    if(pos.row == -1) return;
  }
}

void game()
{
	char board[8][4];
	printf("The game has started\n");
  game_init_board(board);
  game_update(board);
	printf("The game has finished\n");
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
	game();
}
