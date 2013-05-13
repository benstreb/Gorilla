#include "ai_monte_carlo.h"

#include <vector>
#include <map>
#include <utility>
#include <iostream>
#include <time.h> 


AI_Monte_Carlo::AI_Monte_Carlo(int player, int games_, int turns_) : AI(player){
  std::cout << "Made a Monte Carlo AI as player" << player << std::endl;
  if(games < 1) {
    games = 1;
  }
  if(turns < 1) {
    turns = 1;
  }
  games = games_;
  turns = turns_;
  
}

AI_Monte_Carlo::~AI_Monte_Carlo(){
  std::cout << "deleting monte carlo AI" << std::endl;
}

coord AI_Monte_Carlo::getMove(GoBoard* board) {
  srand(time(NULL));
  
  //std::cout << "moves for player: " << player << std::endl;
  coordList moves;
  board->getAllReasonableMoves(player, moves);
  std::cout<<moves.numCoords<<" moves left\n";
  int num_moves = moves.numCoords;
  
  coord choice(-1,-1);
  double best_state = -999;
  
  if(num_moves == 0) {
    std::cout << "NO available moves, pass" << std::endl;
  } else {
    
    //For each reasonable move
    for(int e = 0; e < moves.numCoords; ++e) {
      coord this_move = moves.getMove(e);
      double avg_score = evaluateMove(board, player, this_move);
      //double avg_score = !e ? 1.0 : 0;
    
      if(avg_score > best_state) {
        best_state = avg_score;
        choice = this_move;
      }
    }  
    /////
    std::cout << "Player " << player << " chose move: " << choice.x << ", " << choice.y << std::endl;
  }
  return choice;
}

double AI_Monte_Carlo::evaluateMove(GoBoard* board, int player, coord loc) {
  double total_score = 0;
  bool valid = board->placePiece(player, loc.x, loc.y);
  if (!valid) {
    std::cout << "Evaluating an illegal move. What?" << std::endl;
    return -1;
  }
  coordList test_moves;
  for (int sim = 0; sim < games; ++sim) {
    GoBoard testBoard(*board);
    int current_player = player;
    
    //For the specified number of turns
    for (int t = 0; t < turns; ++t) {
      testBoard.getAllReasonableMoves(current_player, test_moves);
      int num_test_moves = test_moves.numCoords;
        
      coord test_choice(-1,-1);
        
      if (num_test_moves != 0) {
        test_choice = test_moves.getMove(rand() % num_test_moves);
        testBoard.placePiece(current_player, test_choice.x, test_choice.y);
      }
      
      testBoard.nextTurn();
      current_player*=-1;
    }
    total_score += testBoard.getBoardStateForPlayer(player);
    total_score -= testBoard.getBoardStateForPlayer(player*-1);
  }
  board->removePiece(loc.x, loc.y);
  double avg_score = total_score / (double)games;
  //std::cout << avg_score;
  return avg_score;
}


