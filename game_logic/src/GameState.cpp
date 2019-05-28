#include <GameState.hpp>

bool GameState::players_assigned()
{
   return (players.at(0) != 0 && players.at(1) != 0);
}

char GameState::id_to_team(std::size_t id)
{
   if(players.at(0) == id)
   {
      printf("Id %d is BLACK\n", id);
      return BLACK;
   }
   if(players.at(1) == id)
   {
      printf("Id %d is WHITE\n", id);
      return WHITE;
   }
   printf("Id %d is not registered\n", id);
   return EMPTY;
}

GameState::GameState()
{
  printf("Starting the game\n");
  init_board();
}

GameState::~GameState(){}

void GameState::init_board()
{
   black_move = true;
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

std::pair<char, bool> GameState::register_player(std::size_t id)
{
   std::lock_guard<std::mutex> lock(mtx);
   std::pair<char, bool> out = { EMPTY, false };
   if(players.at(0) == 0)
   {
      printf("Player with id %d plays for the BLACK team\n", id);
      players.at(0) = id;
      out.first = BLACK;
   }
   else if(players.at(1) == 0)
   {
      printf("Player with id %d plays for the WHITE team\n", id);
      players.at(1) = id;
      out.first = WHITE;
   }
   if(players_assigned())
   {
      out.second = true;
   }
   return out;
}

bool GameState::unregister_player(std::size_t id)
{
   if(players.at(0) == id)
   {
      players.at(0) = 0;
      return true;
   }
   if(players.at(1) == id)
   {
      players.at(1) = 0;
      return true;
   }
   return false;
}

char GameState::move(std::size_t id, position pos, const std::vector<char>& moves)
{
   if((id_to_team(id) == BLACK) != black_move)
   {
      printf("Can not move: not your turn\n");
      return MOVE_WRONG_TEAM;
   }

   auto check_position = [&](position pos)
   {
      if(
         pos.row < 0 ||
         pos.row > 7 ||
         pos.element < 0 ||
         pos.element > 3 )
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

   if(!check_position(pos)) return MOVE_ILLEGAL;

   char * tile_origin = tile(pos);
   if(tile_team(*tile_origin) == EMPTY)
   {
      printf("Can not move: the tile is empty\n");
      return MOVE_ILLEGAL;
   }

   if((tile_team(*tile_origin) == BLACK) != black_move)
   {
      printf("Can not move: trying to move the opposite team\n");
      return MOVE_WRONG_TEAM;
   }

   auto pos_new = pos;
   pos_new.move(moves[0]);

   if(!check_position(pos_new)) return MOVE_ILLEGAL;

   char * tile_target = tile(pos_new);

   if(tile_team(*tile_target) != EMPTY)
   {
      printf("Target tile is occupied, can you eat it?\n");

      if((tile_team(*tile_target) == BLACK) == black_move)
      {
         printf("Target tile is friendly, can't eat\n");
         return MOVE_ILLEGAL;
      }

      printf("Target tile is enemy, trying to eat\n");
      auto pos_eaten = pos_new;
      pos_new.move(moves[0]);

      if(!check_position(pos_new)) return MOVE_ILLEGAL;
      tile_target = tile(pos_new);

      if(tile_team(*tile_target) != EMPTY)
      {
         printf("Can not move: target tile is occupied\n");
         return MOVE_ILLEGAL;
      }
      *tile(pos_eaten) = EMPTY;
      printf("Ate the enemy\n");

   }
   *tile_target = *tile_origin;
   *tile_origin = EMPTY;
   black_move = !black_move;
   return MOVE_SUCCESS;
}
