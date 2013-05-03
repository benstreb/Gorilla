#ifndef _GO_BOARD_H_
#define _GO_BOARD_H_

#include <utility>

#include <vector>
#include <map>

typedef std::pair<int,int> coord;
typedef std::pair<coord, int> PlacedPieceType;

const int BOARD_SIZE = 9;

const int PLAYER_1 = 1;
const int PLAYER_2 = -1;

const float BOARD_HEIGHT = 1.0f;
const float BOARD_MIN = 0.0f;
const float BOARD_MAX = 1.0f;
const float BOARD_GRID_SPACING = 1/.9f;
const float PIECE_OFFSET = 0.005f;
const float PIECE_X_SCALE = 0.1f;
const float PIECE_Y_SCALE = 0.03f;
const float PIECE_Z_SCALE = 0.1f;

class GoBoard {
public:

GoBoard();
~GoBoard();

//Accessors
int getPiece(int x, int y) const {return pieces[x][y];}
double getControl(int x, int y) const {return control[x][y];}

//Modifiers 
bool placePiece(int player, int x, int y);

//Misc
bool legalMove(int player, int x, int y);
//Modifiers 
bool placePiece(int player, int x, int y);
bool removePiece(int x, int y);
bool removePieces(std::vector<coord>& toRemove);
bool addPieces(std::vector<coord>& toAdd, int player);

void printControl();
void printBoard();

std::pair<std::pair<int, int>, int> getSpeculativePiece() const;
void placeSpeculativePiece(int player, int x, int y);
int getPlayerScore(int player);

void printControl();
void printBoard();
void printPrevBoard();
void printPrev2Board();
void printScoreBoard();

void endOfGame();

//Helper
private:
void updateControlMap(int player, int x, int y);
void eraseControlMap();
void updateBoard();
void updatePrevBoard();

void fillLine(coord start, coord dir);
void calculateScores();

bool isKo();

std::vector<coord>* getDeadPiecesForPlayer(int player);
bool stillAlive(coord piece);

int getPlayerAtCoord(coord place);

//Representation
int 	pieces[BOARD_SIZE][BOARD_SIZE];
int		prev_pieces[BOARD_SIZE][BOARD_SIZE];
int		prev2_pieces[BOARD_SIZE][BOARD_SIZE];
int 	score_board[BOARD_SIZE][BOARD_SIZE];

int nextPieceID;

int player1_score;
int player2_score;

std::map<coord, int>	placed_pieces;
std::map<int, int> placed_pieces_player;

std::pair<std::pair<int, int>, int> speculativePiece;

};

#endif
