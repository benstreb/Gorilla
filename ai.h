#ifndef _GO_AI_H_
#define _GO_AI_H_

#include "GoBoard.h"

class AI{
	
	public:
	int player;
	
	//AI(){}
	AI(int playingAs){player = playingAs;}
	~AI(){};
	
	virtual coord getMove(GoBoard* board) = 0;
		
};


#endif
