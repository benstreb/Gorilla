#ifndef _GO_BOARD_H_
#define _GO_BOARD_H_

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
  bool placePiece(int x, int y, int player);

  //Misc
  bool legalMove(int player, int x, int y);

  void printControl();
  void printBoard();

  //Helper
private:
  void updateControlMap(int player, int x, int y);
  void updateBoard();

  int pieces[BOARD_SIZE][BOARD_SIZE];
  int prev_pieces[BOARD_SIZE][BOARD_SIZE];

  double control[BOARD_SIZE][BOARD_SIZE];

};

#endif
