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

char * GameState::tile(const position & pos)
{
  return &board[(int)pos.row][(int)pos.element];
}

void GameState::move(position pos, const std::vector<char>& moves)
{
  auto check_position = [&](position pos)
  {
    if(pos.row == -1 || pos.element == -1)
    {
      printf("Can not move: target tile is outside the board\n");
      return false;
    }
    return true;
  };

  auto tile_team = [&](char tile) -> char
  {
      return tile & TEAM_MASK;
  };


  const char* dir_names[]
  {
    "UP LEFT",
    "UP RIGHT",
    "DOWN LEFT",
    "DOWN RIGHT",
  };

  printf("Moving %d:%d %s\n", pos.row, pos.element, dir_names[(int)moves[0]]);

  if(!check_position(pos)) return;

  char * tile_origin = tile(pos);
  if(tile_team(*tile_origin) == EMPTY)
  {
    printf("Can not move: the tile is empty\n");
    return;
  }

  if((tile_team(*tile_origin) == BLACK) != black_move)
  {
    printf("Can not move: Wrong team\n");
    return;
  }

  auto pos_new = pos;
  pos_new.move(moves[0]);

  if(!check_position(pos_new)) return;

  char * tile_target = tile(pos_new);

  if(tile_team(*tile_target) != EMPTY)
  {
    printf("Target tile is occupied, can you eat it?\n");

    if((tile_team(*tile_target) == BLACK) == black_move)
    {
      printf("Target tile is friendly, can't eat\n");
      return;
    }

    printf("Target tile is enemy, trying to eat\n");
    auto pos_eaten = pos_new;
    pos_new.move(moves[0]);

    if(!check_position(pos_new)) return;
    tile_target = tile(pos_new);

    if(tile_team(*tile_target) != EMPTY)
    {
      printf("Can not move: target tile is occupied\n");
      return;
    }
    *tile(pos_eaten) = EMPTY;
    printf("Ate the enemy\n");

  }
  *tile_target = *tile_origin;
  *tile_origin = EMPTY;
  black_move = !black_move;
}
