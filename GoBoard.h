#ifndef _GO_BOARD_H_
#define _GO_BOARD_H_

#include <utility>
#include <vector>
#include <map>

typedef std::pair<int,int> coord;
typedef std::pair<coord, int> PlacedPieceType;

const int BOARD_SIZE = 8;

const int PLAYER_1 = 1;
const int PLAYER_2 = -1;

class GoBoard{

public:

GoBoard();
~GoBoard();

//Accessors
int getPiece(int x, int y) const {return pieces[x][y];}
double getControl(int x, int y) const {return control[x][y];}

//Modifiers 
bool placePiece(int player, int x, int y);
bool removePiece(int x, int y);
bool removePieces(std::vector<coord>& toRemove);

//Misc
bool legalMove(int player, int x, int y);

void printControl();
void printBoard();

//Helper
private:
void updateControlMap(int player, int x, int y);
void eraseControlMap();
void updateBoard();


std::vector<coord>* getDeadPiecesForPlayer(int player);
bool stillAlive(coord piece);

int getPlayerAtCoord(coord place);

//Representation
int 	pieces[BOARD_SIZE][BOARD_SIZE];
int		prev_pieces[BOARD_SIZE][BOARD_SIZE];

int nextPieceID;

std::map<coord, int>	placed_pieces;
std::map<int, int> placed_pieces_player;


double	control[BOARD_SIZE][BOARD_SIZE];

};

#endif
