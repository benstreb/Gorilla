#include "GoBoard.h"

#include <vector>
#include <queue>
#include <utility>
#include <map>
#include <math.h>
#include <iostream>
#include <assert.h>


typedef std::pair<coord, bool> visitedPair;

typedef std::pair<coord, float> waveFrontPair;
typedef std::map<coord, bool> waveFrontType;

typedef std::pair<coord, int> chainPair;

const double CONTROL_EPSILON = .1;


GoBoard::GoBoard()
{
	turn = -1;
	nextPieceID = 0;
	for(int j = 0; j<BOARD_SIZE; ++j)
	{
		for(int i = 0; i < BOARD_SIZE; ++i)
		{
			pieces[i][j] = 0;
			prev_pieces[i][j]=0;
			prev2_pieces[i][j]=0;
			control[i][j] = 0;
			score_board[i][j] = 0;
		}
		//std::cout << std::endl;
	}
	
	player1_score = -1;
	player2_score = -1;
}

GoBoard::GoBoard( const GoBoard& other )
{
	for(int i = 0; i < BOARD_SIZE; ++i)
	{
		for(int j = 0; j < BOARD_SIZE; ++j)
		{
			pieces[i][j] = other.pieces[i][j];
			prev_pieces[i][j] = other.prev_pieces[i][j];
			prev2_pieces[i][j] = other.prev2_pieces[i][j];
			score_board[i][j] = other.score_board[i][j];
			control[i][j] = other.control[i][j];
		}
	}
	
	nextPieceID = other.nextPieceID;
	player1_score = other.player1_score;
	player2_score = other.player2_score;
	turn = other.turn;
	
	placed_pieces = other.placed_pieces;
	placed_pieces = other.placed_pieces;
	
	placed_pieces_player = other.placed_pieces_player;
	
	speculativePiece = other.speculativePiece;
}

GoBoard::~GoBoard(){}



//Accessors


//Modifers
bool GoBoard::placePiece(int player, int x, int y)
{
	assert(player == -1 || player == 0 || player == 1);
	if(player == 0) return false;
	int temp[BOARD_SIZE][BOARD_SIZE];	
	for(int i = 0; i < BOARD_SIZE; ++i)
	{
		for(int j = 0; j < BOARD_SIZE; ++j)
		{
			temp[i][j] = prev_pieces[i][j];
		}
	}
	updatePrevBoard();
	
	if(legalMove(player, x, y))
	{
		pieces[x][y] = player;
		int newPieceID = nextPieceID;
		nextPieceID++;
		coord newCord = std::make_pair(x,y);
		placed_pieces.insert(std::make_pair(newCord, newPieceID));
		placed_pieces_player.insert(std::make_pair(newPieceID, player));
		
		//Check for dead enemy pieces
		std::vector<coord>* dead_enemy_pieces = getDeadPiecesForPlayer(player*-1);
		int num_dead_enemies = dead_enemy_pieces->size();
		if( num_dead_enemies > 0)
		{
			removePieces(*dead_enemy_pieces);
			if(num_dead_enemies == 1)
			{
				//std::cout << "Check for Ko!!!!!!!!!!!!!!!!!: " << std::endl;
				//check for KO
				bool is_ko = true;
				for(int i = 0; i < BOARD_SIZE; ++i)
				{
					for(int j = 0; j < BOARD_SIZE; ++j)
					{
						if(pieces[i][j] != temp[i][j])
						{
							is_ko = false;
						}
					}
				}
				if(is_ko)
				{
					//std::cout << "THIS IS Ko!!!!!!!!!!!!!!!!!: " << std::endl;
					//ROLLBACK
					auto piece = std::vector<coord>();
					piece.push_back(coord(x, y));
					removePieces(piece);
					addPieces(*dead_enemy_pieces, player*-1);	
					for(int i = 0; i < BOARD_SIZE; ++i)
					{
						for(int j = 0; j < BOARD_SIZE; ++j)
						{
							prev_pieces[i][j] = temp[i][j];
						}
					}
					delete dead_enemy_pieces;
					return false;
				}
			}
		}
		else
		{
			//check for dead ally pieces
			std::vector<coord>* dead_ally_pieces = getDeadPiecesForPlayer(player);
			int num_dead_ally = dead_ally_pieces->size();
			delete dead_ally_pieces;
			//If suicidal move
			if( num_dead_ally > 0)
			{
				//rollback
				auto piece = std::vector<coord>();
				piece.push_back(coord(x, y));
				removePieces(piece);		
				for(int i = 0; i < BOARD_SIZE; ++i)
				{
					for(int j = 0; j < BOARD_SIZE; ++j)
					{
						prev_pieces[i][j] = temp[i][j];
					}
				}
				delete dead_enemy_pieces;
				return false;
			}
			//else was a legal move
			updateControlMap(player, x, y);
		}
		delete dead_enemy_pieces;
		
		//update the prevprev board
		for(int i = 0; i < BOARD_SIZE; ++i)
		{
			for(int j = 0; j < BOARD_SIZE; ++j)
			{
				prev2_pieces[i][j] = temp[i][j];
			}
		}
		return true;
	}
	
	//rollback
	for(int i = 0; i < BOARD_SIZE; ++i)
	{
		for(int j = 0; j < BOARD_SIZE; ++j)
		{
			prev_pieces[i][j] = temp[i][j];
		}
	}
	
	return false;
}

bool GoBoard::addPieces(std::vector<coord>& toAdd, int player)
{
	//add pieces back to the control board
	for( std::vector<coord>::iterator iter = toAdd.begin(); iter!=toAdd.end(); iter++)
	{
		if(!placePiece(player,iter->first,iter->second))
		{
			std::cout << "ERROR ADDING PIECES";
			return false;
		}
	}
	return true;
}

bool GoBoard::removePieces(std::vector<coord>& toRemove)
{
	for(unsigned int i = 0; i < toRemove.size(); ++i)
	{
		coord piece = toRemove[i];
		if(!removePiece(piece.first, piece.second))
		{
			std::cout << "ERROR REMOVING PIECES" << std::endl;
			return false;
		}
	}
	
	eraseControlMap();
	
	//add remaining pieces back to the control board
	for( std::map<coord,int>::iterator itr = placed_pieces.begin(); itr!=placed_pieces.end(); itr++)
	{
		int player = ((placed_pieces_player.find(itr->second))->second);
		coord place = itr->first;
		updateControlMap(player,place.first, place.second);
		
		//std::cout << "=======================" << std::endl; 
		//printControl();
	}
	return true;
}

bool GoBoard::removePiece(int x, int y)
{
	if(pieces[x][y] == 0)
	{
		return false;
	}
	else
	{
		pieces[x][y] = 0;
		std::map<coord,int>::iterator piece = placed_pieces.find(std::make_pair(x,y));
		if(piece == placed_pieces.end())
		{
			return false;
		}
		placed_pieces_player.erase(piece->second);
		placed_pieces.erase(piece->first);
		return true;
	}
}

int GoBoard::getPlayerScore(int player)
{
	if(player == 1)
	{
		return player1_score;
	}
	else if(player == -1)
	{
		return player2_score;
	}
	std::cout << "Invalid Player" << std::endl;
	return -2;
}

//Misc
bool GoBoard::legalMove(int player, int x, int y)
{
	assert(player == -1 || player == 0 || player == 1);
	if(player != 0
		&& x >= 0
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
			std::cout << control[i][j] << "\t";
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
			std::cout << pieces[i][j] << "\t";
		}
		std::cout << std::endl;
	}
}

void GoBoard::printPrevBoard()
{
	for(int j = 0; j<BOARD_SIZE; ++j)
	{
		for(int i = 0; i < BOARD_SIZE; ++i)
		{
			std::cout << prev_pieces[i][j] << "\t";
		}
		std::cout << std::endl;
	}
}

void GoBoard::printPrev2Board()
{
	for(int j = 0; j<BOARD_SIZE; ++j)
	{
		for(int i = 0; i < BOARD_SIZE; ++i)
		{
			std::cout << prev2_pieces[i][j] << "\t";
		}
		std::cout << std::endl;
	}
}

void GoBoard::printScoreBoard()
{
	std::cout << "SCORE BOARD" << std::endl;
	for(int j = 0; j<BOARD_SIZE; ++j)
	{
		for(int i = 0; i < BOARD_SIZE; ++i)
		{
			std::cout << score_board[i][j] << "\t";
		}
		std::cout << std::endl;
	}
}

void GoBoard::endOfGame()
{
	calculateScores();
}

//Helper

void GoBoard::calculateScores()
{
	for(std::map<coord,int>::iterator itr = placed_pieces.begin();
							itr != placed_pieces.end();
							itr++)
	{
		coord next = itr->first;
		
		coord up = std::make_pair(next.first, next.second-1);
		coord down = std::make_pair(next.first, next.second+1);
		coord right = std::make_pair(next.first+1, next.second);
		coord left = std::make_pair(next.first-1, next.second);
		
		fillLine(itr->first, up);
		fillLine(itr->first, right);
		fillLine(itr->first, down);
		fillLine(itr->first, left);
	}
	
	player1_score = 0;
	player2_score = 0;
	
	for(int i = 0; i < BOARD_SIZE; ++i)
	{
		for(int j = 0; j < BOARD_SIZE; ++j)
		{
			if(score_board[i][j] <= -1)
			{
				player2_score += 1;
			}
			else if(score_board[i][j] >= 1)
			{
				player1_score += 1;
			}
		}
	}
	
	printScoreBoard();
}

void GoBoard::updateControlMap(int player, int x, int y)
{
	assert (player == -1 || player == 0 || player == 1);
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
				if((placed_pieces.find(next_loc)) == placed_pieces.end())
				{
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
}

void GoBoard::updatePrevBoard(){
	for(int i = 0; i < BOARD_SIZE; ++i)
	{
		for(int j = 0; j < BOARD_SIZE; ++j)
		{
			prev_pieces[i][j] = pieces[i][j];
		}
	}
}

void GoBoard::fillLine(coord start, coord dir)
{
	coord current = start;
	int player = getPlayerAtCoord(start);
	
	std::cout << player;
	
	while(placed_pieces.find(current) == placed_pieces.end())
	{
		int x = current.first;
		int y = current.second;
		
		if(x >= 0
		&& x < BOARD_SIZE
		&& y >= 0
		&& y < BOARD_SIZE)
		{
			score_board[x][y] += player;
			current.first += dir.first;
			current.second += dir.second;
		}
		else
		{
			break;
		}
	}
}

bool GoBoard::isKo()
{
	for(int i = 0; i < BOARD_SIZE; ++i)
	{
		for(int j = 0; j < BOARD_SIZE; ++j)
		{
			if(pieces[i][j] != prev2_pieces[i][j])
			{
				return false;
			}
		}
	}
	return true;
}

void GoBoard::eraseControlMap(){
	for(int i = 0; i < BOARD_SIZE; ++i)
	{
		for(int j = 0; j < BOARD_SIZE; ++j)
		{
			control[i][j] = 0;
		}
	}
}

//IMPORTANT: VECTOR PASSED FROM THIS FUNCTION NEEDS TO BE HANDLED!
std::vector<coord>* GoBoard::getDeadPiecesForPlayer(int player)
{
	std::vector<coord>* dead = new std::vector<coord>;
	for(std::map<coord, int>::iterator itr = placed_pieces.begin(); 
									itr != placed_pieces.end();
									itr++)
	{
		coord checkingCoord = itr->first;
		if(getPlayerAtCoord(checkingCoord) == player)
		{
			if(!stillAlive(checkingCoord))
			{
				dead->push_back(checkingCoord);
			}
		}
	}
	
	return dead;
}

bool GoBoard::stillAlive(coord piece)
{
	std::queue<coord> chain;
	waveFrontType visited;
	
	int thisPlayer = getPlayerAtCoord(piece);
	
	chain.push(piece);
	visited.insert(std::pair<coord,bool>(piece, true) );
	
	while(!chain.empty())
	{
		coord newest = chain.front();
		coord next = newest;
		chain.pop();
		
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
				//If open square, the chain is still alive
				if(pieces[next_loc.first][next_loc.second] == 0)
				{
					return true;
				}
				else if(pieces[next_loc.first][next_loc.second] == thisPlayer)
				{
					chain.push(next_loc);
					visited.insert(std::pair<coord,bool>(next_loc, true) );
				}
			}
		}
	}
	return false;
}

int GoBoard::getPlayerAtCoord(coord place)
{
	return(placed_pieces_player.find(placed_pieces.find(place)->second)->second);
}

std::pair<std::pair<int, int>, int> GoBoard::getSpeculativePiece() const
{
	return speculativePiece;
}

bool GoBoard::applySpeculativePiece()
{
	bool success = placePiece(turn, speculativePiece.first.first, speculativePiece.first.second);
	if (success)
	{
		turn *= -1;
	}
	return success;
}
void GoBoard::placeSpeculativePiece(int player, int x, int y)
{
	if (player == 0) return;
	assert(player == -1 || player == 1);
	speculativePiece = std::make_pair(std::make_pair(x, y), player);
}
