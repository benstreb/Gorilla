#ifndef _GO_AI_RANDOM_H_
#define _GO_AI_RANDOM_H_

#include "ai.h"

class AI_Random : public AI{
	
	public:
	
	//AI_Random();
	AI_Random(int playingAs);
	~AI_Random();
	
	virtual coord getMove(GoBoard* board);


};

#endif
