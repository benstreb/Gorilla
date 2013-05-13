#include "ai_random.h"

#include <vector>
#include <map>
#include <utility>
#include <iostream>
#include <time.h> 


AI_Random::AI_Random(int player) : AI(player) {
  std::cout << "Made a random AI as player " << player << std::endl;
}

AI_Random::~AI_Random() {
  std::cout << "deleting random AI" << std::endl;
}

coord AI_Random::getMove(GoBoard* board) {
  srand (time(NULL));
  
  //std::cout << "moves for player: " << player << std::endl;
  coordList moves;
  board->getAllReasonableMoves(player, moves);
  int num_moves = moves.numCoords;
  
  coord choice(-1,-1);
  
  if(num_moves == 0) {
    std::cout << "NO available moves, pass" << std::endl;
  } else {
    choice = moves.getMove(rand() % num_moves);
    std::cout << "Player " << player << " chose move: " << choice.x << ", " << choice.y << std::endl;
  }

  return choice;
}


