#include <GameState.hpp>

GameState::GameState()
{
  printf("Starting the game\n");
  init_board();
}

GameState::~GameState(){}

void GameState::init_board()
{
  auto init_row = [](char row[], char man)
  {
    for(int i = 0; i < 4; i++)
      row[i] = man;
  };

  for(int i = 0; i < 3; i++)
  {
    init_row(board[i],   BLACK);
    init_row(board[7-i], WHITE);
  }
  init_row(board[3], EMPTY);
  init_row(board[4], EMPTY);
}

char & GameState::tile(const position & pos)
{
  return board[pos.row][pos.element];
}

void GameState::move(position pos, const std::vector<char>& moves)
{
  char &tile_origin = tile(pos);
  if((tile_origin & TEAM_MASK) == EMPTY)
  {
    printf("Can not move: the tile is empty\n");
    return;
  }

  auto pos_new = pos;
  pos_new.move(moves[0]);

  if(pos_new.row == -1 || pos_new.element == -1)
  {
    printf("Can not move: target tile is outside the board\n");
    return;
  }

  char &tile_target = tile(pos_new);

  if((tile_target & TEAM_MASK) != EMPTY)
  {
    printf("Can not move: target tile is occupied\n");
    return;
  }
  tile_target = tile_origin;
  tile_origin = EMPTY;
}
