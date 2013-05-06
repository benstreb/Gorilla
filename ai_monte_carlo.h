#ifndef _GO_AI_CARLO_H_
#define _GO_AI_CARLO_H_

#include "ai.h"

class AI_Monte_Carlo : public AI{
	
	public:
	
	AI_Monte_Carlo(int playingAs, int games_ = 1, int turns_ = 1);
	~AI_Monte_Carlo();
	
	virtual coord getMove(GoBoard* board);
	
	int games;
	int turns;

};

#endif
