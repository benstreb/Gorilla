#include "ai_random.h"

#include <vector>
#include <map>
#include <utility>
#include <iostream>
#include <time.h> 


AI_Random::AI_Random(int player) : AI(player){
	std::cout << "Made a random AI with player" << std::endl;
}

AI_Random::~AI_Random(){
	std::cout << "deleting random AI" << std::endl;
}

coord AI_Random::getMove(GoBoard* board)
{
	srand (time(NULL));
	int x = -1;
	int y = -1;
	
	bool success = false;
	while(!success){
		GoBoard testBoard(*board);
		
		x = rand() % BOARD_SIZE;
		y = rand() % BOARD_SIZE;
		
		//std::cout << "PLAYER: " << player << " TRYING: " << x << ", " << y << std::endl;
		
		success = testBoard.placePiece(player, x, y);
		/*
		if(success)
		{
			std::cout << "Value for AI: " << testBoard.getBoardStateForPlayer(player) << std::endl;	
		}
		*/
		
	}
	return std::make_pair(x,y);
}


