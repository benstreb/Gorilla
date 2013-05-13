#ifndef _GO_BOARD_H_
#define _GO_BOARD_H_

#include <utility>

#include <vector>
//#include <map>
#include <unordered_map>
#include <assert.h>
#include <array>

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

struct coord {
  int x;
  int y;
  coord() : x(0), y(0) {}
  coord(int _x, int _y) : x(_x), y(_y) {}
  bool operator== (const coord other) const { return x == other.x && y == other.y; }
  bool operator< (const coord other) const {
    if (x == other.x) {
      return y < other.y;
    } else {
      return x < other.x;
    }
  }
};
template<>
struct std::hash<coord> {
  size_t operator()(const coord &val) const {
    return val.x*BOARD_SIZE + val.y;
  }
};

typedef std::pair<coord, int> PlacedPieceType;

struct coordList {
  int numCoords;
  int moveArray[BOARD_SIZE*BOARD_SIZE*2];

  coordList() : numCoords(0) {}

  int getX(int i) {
    return moveArray[i*2];
  }
  int getY(int i) {
    return moveArray[i*2+1];
  }
  coord getMove(int i) {
    return coord(moveArray[i*2], moveArray[i*2+1]);
  }
  void putMove(int x, int y) {
    //printf("%d\n", numMoves);
    assert(numCoords*2+1 < BOARD_SIZE*BOARD_SIZE*2);
    moveArray[numCoords*2] = x;
    moveArray[numCoords*2+1] = y;
    ++numCoords;
  }
  void empty() {
    numCoords = 0;
  }
  void emptyTo(int length) {
    assert(length <= numCoords);
    numCoords = length;
  }
};

class GoBoard {
public:

  GoBoard();
  GoBoard(const GoBoard& other);
  ~GoBoard();

  //Accessors
  int getTurn(){return turn;}
  int getPiece(int x, int y) const {return pieces[x][y];}
  double getControl(int x, int y) const {return control[x][y];}
  double getBoardStateForPlayer(int player);
  bool getJustPassed(){return just_passed;}

  //Modifiers 
  bool placePiece(int player, int x, int y);
  void passTurn(){turn *= -1; just_passed = true;}
  void nextTurn(){turn *= -1; just_passed = false;}

  //Misc
  bool legalMove(int player, int x, int y);

  //similar to above, but does not play in single-point eyes
  void getAllReasonableMoves(int player, coordList &coordList);

  //Modifiers 
  bool removePiece(int x, int y);
  bool removePieces(coordList &toRemove);
  bool addPieces(coordList &toAdd, int player);

  void printControl();
  void printBoard();

  std::pair<coord, int> getSpeculativePiece() const;
  bool applySpeculativePiece();
  void placeSpeculativePiece(int player, int x, int y);
  int getPlayerScore(int player);

  void printPrevBoard();
  void printPrev2Board();
  void printScoreBoard();

  int endOfGame();

  //Helper
private:
  void updateControlMap(int player, int x, int y);
  void eraseControlMap();
  void updateBoard();
  void updatePrevBoard();

  void fillLine(coord start, coord dir);
  void calculateScores();

  bool isKo();
  bool isMyPiece(int player, int x, int y) { return pieces[x][y] == player; }

  int getDeadPiecesForPlayer(int player, coord last_move, coordList &dead_pieces);
  int checkChainForDeadPieces(coord piece, int (&visited)[BOARD_SIZE][BOARD_SIZE], int already_visited, coordList &pieces);
  //bool stillAlive(coord piece);

  int getPlayerAtCoord(coord place);

  //Representation
  int pieces[BOARD_SIZE][BOARD_SIZE];
  int prev_pieces[BOARD_SIZE][BOARD_SIZE];
  int prev2_pieces[BOARD_SIZE][BOARD_SIZE];
  int score_board[BOARD_SIZE][BOARD_SIZE];
  double control[BOARD_SIZE][BOARD_SIZE];

  int nextPieceID;

  int player1_score;
  int player2_score;

  int turn;
  bool just_passed;

  std::unordered_map<coord, int> placed_pieces;
  std::unordered_map<int, int> placed_pieces_player;

  std::pair<coord, int> speculativePiece;
};

#endif
