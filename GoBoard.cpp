#include "GoBoard.h"

#include <queue>
#include <vector>
#include <utility>
#include <map>
#include <math.h>
#include <iostream>

typedef std::pair<int,int> coord;
typedef std::pair<coord, bool> visitedPair;
typedef std::pair<coord, float> waveFrontPair;
typedef std::map<coord, bool> waveFrontType;

const double CONTROL_EPSILON = .1;


GoBoard::GoBoard(){
	for(int j = 0; j<BOARD_SIZE; ++j)
	{
		for(int i = 0; i < BOARD_SIZE; ++i)
		{
			pieces[i][j] = 0;
			prev_pieces[i][j]=0;
			control[i][j] = 0;
		}
		std::cout << std::endl;
	}
}

GoBoard::~GoBoard(){}

//Accessors


//Modifers
bool GoBoard::placePiece(int player, int x, int y)
{
	if(legalMove(player, x, y))
	{
		pieces[x][y] = player;
		updateControlMap(player, x, y);
		return true;
	}
	
	return false;
}

//Misc
bool GoBoard::legalMove(int player, int x, int y)
{	
	if(x >= 0
		&& x < BOARD_SIZE
		&& y >= 0
		&& y < BOARD_SIZE
		&& pieces[x][y] == 0)
		{
			return true;
		}
	return false;
}

void GoBoard::printControl()
{
	for(int j = 0; j<BOARD_SIZE; ++j)
	{
		for(int i = 0; i < BOARD_SIZE; ++i)
		{
			std::cout << control[i][j] << " ";
		}
		std::cout << std::endl;
	}
}

void GoBoard::printBoard()
{
	for(int j = 0; j<BOARD_SIZE; ++j)
	{
		for(int i = 0; i < BOARD_SIZE; ++i)
		{
			std::cout << pieces[i][j] << " ";
		}
		std::cout << std::endl;
	}
}

//Helper
void GoBoard::updateControlMap(int player, int x, int y)
{
	std::queue<waveFrontPair> waveFront;
	waveFrontType visited;

	control[x][y] += 1.0*player;
	if(fabs(control[x][y]) < 1.0)
	{
		control[x][y] = 1.0*player;
	}
	
	coord here = std::make_pair(x,y);
	waveFrontPair firstEnergy = std::make_pair(here,1.0*player);
	
	waveFront.push(firstEnergy);
	visited.insert(std::pair<coord,bool>(here, true) );
	
	while(!waveFront.empty())
	{
		waveFrontPair newest = waveFront.front();
		coord next = newest.first;
		waveFront.pop();
		
		double current_power = newest.second;
		double next_power = current_power/2.0;
		
		coord up = std::make_pair(next.first, next.second-1);
		coord down = std::make_pair(next.first, next.second+1);
		coord right = std::make_pair(next.first+1, next.second);
		coord left = std::make_pair(next.first-1, next.second);
		
		std::vector<coord> next_locs;
		next_locs.push_back(up);
		next_locs.push_back(down);
		next_locs.push_back(left);
		next_locs.push_back(right);
		
		for(unsigned int i=0; i< next_locs.size(); ++i)
		{		
			coord next_loc = next_locs[i];
			if(next_loc.first >= 0 
				&& next_loc.first < BOARD_SIZE
				&& next_loc.second >= 0 
				&& next_loc.second < BOARD_SIZE
				&& visited.find(next_loc) == visited.end())
			{
				double result_power = next_power;
				//If opponents space
				if(control[next_loc.first][next_loc.second]*next_power == -1)
				{
					result_power += control[next_loc.first][next_loc.second];
				}
				control[next_loc.first][next_loc.second] += next_power;
				
				if(fabs(result_power) > CONTROL_EPSILON)
				{
					waveFront.push(std::make_pair(next_loc,next_power));
					visited.insert(std::pair<coord,bool>(next_loc, true) );
				}
			}
		}
	}
}
