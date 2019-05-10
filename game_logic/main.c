#include <stdio.h>
#include <stdlib.h>

#define EMPTY 0x0
#define BLACK 0x1
#define WHITE 0x3
#define QUEEN 0x4

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
		game_init_row(board[i], BLACK);
		game_init_row(board[7-i], WHITE);
	}
   game_init_row(board[3], EMPTY);
   game_init_row(board[4], EMPTY);
}

void game_print_board(char board[8][4])
{
	int row, el;
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

void game()
{
	char board[8][4];
	printf("The game has started\n");
   game_init_board(board);
   game_print_board(board);
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
