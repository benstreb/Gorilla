#include "ai_monte_carlo.h"

#include <vector>
#include <map>
#include <utility>
#include <iostream>
#include <time.h> 


AI_Monte_Carlo::AI_Monte_Carlo(int player, int games_, int turns_) : AI(player){
	std::cout << "Made a Monte Carlo AI as player" << player << std::endl;
	if(games < 1)
	{
		games = 1;
	}
	if(turns < 1)
	{
		turns = 1;
	}
	games = games_;
	turns = turns_;
	
}

AI_Monte_Carlo::~AI_Monte_Carlo(){
	std::cout << "deleting monte carolo AI" << std::endl;
}

coord AI_Monte_Carlo::getMove(GoBoard* board)
{
	srand (time(NULL));
	
	//std::cout << "moves for player: " << player << std::endl;
	std::vector<coord>* moves = board->getAllReasonableMoves(player);
	int num_moves = moves->size();
	
	coord choice = std::make_pair(-1,-1);
	double best_state = -999;
	
	if(num_moves == 0)
	{
		std::cout << "NO available moves, pass" << std::endl;
	}
	else
	{
		
		//For each reasonable move
		for(unsigned int e = 0; e < moves->size(); ++e)
		{
			coord this_move = (*moves)[e];
			double avg_score = evaluateMove(board, player, this_move);
		
			if(avg_score > best_state)
			{
				best_state = avg_score;
				choice = this_move;
			}
		}	
		/////
		std::cout << "Player " << player << " chose move: " << choice.first << ", " << choice.second << std::endl;
	}

	delete moves;
	return choice;
}

double AI_Monte_Carlo::evaluateMove(GoBoard* board, int player, coord loc)
{
	double total_score = 0;
	for(int sim = 0; sim < games; ++sim)
	{
		GoBoard testBoard(*board);
		int current_player = player;
		
		//For the specified number of turns
		for(int t = 0; t < turns; ++t)
		{
			
			//use inital move for first turn
			if(t == 0)
			{
				bool valid = testBoard.placePiece(current_player, loc.first, loc.second);
				if(!valid)
				{
					std::cout << "You can't make this move" << std::endl;
					return -1;
				}
			}
			else
			{
				std::vector<coord>* test_moves = testBoard.getAllReasonableMoves(current_player);
				int num_test_moves = test_moves->size();
				
				coord test_choice = std::make_pair(-1,-1);
				
				if(num_test_moves != 0)
				{
					test_choice = (*test_moves)[rand() % num_test_moves];
					testBoard.placePiece(current_player, test_choice.first, test_choice.second);
				}
			}
			
			testBoard.nextTurn();
			current_player*=-1;
		}
		total_score += testBoard.getBoardStateForPlayer(player);
	}
	double avg_score = total_score / (double)games;
	//std::cout << avg_score;
	return avg_score;
}


