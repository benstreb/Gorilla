#include "GoBoard.h"

#include <vector>
#include <queue>
#include <utility>
//#include <map>
#include <unordered_map>
#include <math.h>
#include <iostream>
#include <assert.h>


typedef std::pair<coord, bool> visitedPair;
typedef std::pair<coord, float> waveFrontPair;
typedef std::unordered_map<coord, bool> waveFrontType;

typedef std::pair<coord, int> chainPair;

const double CONTROL_EPSILON = .1;


GoBoard::GoBoard() {
  turn = -1;
  nextPieceID = 0;
  memset(*pieces, 0, sizeof(pieces));
  memset(*prev_pieces, 0, sizeof(prev_pieces));
  memset(*prev2_pieces, 0, sizeof(prev2_pieces));
  memset(*control, 0, sizeof(control));
  memset(*score_board, 0, sizeof(score_board));
  
  player1_score = -1;
  player2_score = -1;
  
  just_passed = false;
}

GoBoard::GoBoard( const GoBoard& other ) {
  if (&other == this) return;
  memcpy(pieces, other.pieces, sizeof(pieces));
  memcpy(prev_pieces, other.prev_pieces, sizeof(prev_pieces));
  memcpy(prev2_pieces, other.prev2_pieces, sizeof(prev2_pieces));
  memcpy(score_board, other.score_board, sizeof(score_board));
  memcpy(control, other.control, sizeof(control));
  nextPieceID = other.nextPieceID;
  player1_score = other.player1_score;
  player2_score = other.player2_score;
  turn = other.turn;
  
  placed_pieces = other.placed_pieces;
  
  placed_pieces_player = other.placed_pieces_player;
  
  speculativePiece = other.speculativePiece;
}

GoBoard::~GoBoard(){}



//Accessors
double GoBoard::getBoardStateForPlayer(int player) {
  double total = 0;
  for(int i = 0; i < BOARD_SIZE; ++i) {
    for(int j = 0; j < BOARD_SIZE; ++j) {
      double value = control[i][j];
      //std::cout << "Value: " << value << " Player: " << player << std::endl;
      if(value * (double)player >= 0) {
        //std::cout << "!=========adding: " <<  fabs(value) << std::endl;
        total += fabs(value);
      } else {
        //std::cout << "subtracting: " <<  fabs(value) << std::endl;
        total-= fabs(value);
      }
      //std::cout << "current score: " << score << std::endl;
    }
  }
  
  return total;  
}

coord getArbitraryAdjacentSquare(int x, int y) {
  if (x-1 < 0) {
    return coord(x+1, y);
  } else {
    return coord(x-1, y);
  }
}

int GoBoard::scoreFullBoard(int player) {
  int score = 0;
  for(int i = 0; i < BOARD_SIZE; ++i) {
    for(int j = 0; j < BOARD_SIZE; ++j) {
      if (pieces[i][j] == player) {
        score += 1;
      } else if (pieces[i][j] == 0) {
        coord adj = getArbitraryAdjacentSquare(i, j);
        if (pieces[adj.x][adj.y] == player) {
          score += 1;
        }
      }
    }
  }
  return score;
}

//Modifers
bool GoBoard::placePiece(int player, int x, int y, coordList &dead) {
  assert(player == -1 || player == 0 || player == 1);

  if(player == 0) return false;
  if(!legalMove(player, x, y)) return false;

  static int temp[BOARD_SIZE][BOARD_SIZE];
  memcpy(temp, prev_pieces, sizeof(temp));
  memcpy(prev_pieces, pieces, sizeof(prev_pieces));
  //updatePrevBoard();

  pieces[x][y] = player;
  int newPieceID = nextPieceID;
  nextPieceID++;
  coord newCoord(x, y);
  //placed_pieces.insert(std::make_pair(newCord, newPieceID));
  //placed_pieces_player.insert(std::make_pair(newPieceID, player));
    
  //Check for dead enemy pieces
  int num_dead_enemies = getSurroundingDeadPieces(player, newCoord, dead);
  if (num_dead_enemies > 0) {
    if(num_dead_enemies == 1) {
      //std::cout << "Check for Ko!!!!!!!!!!!!!!!!! " << std::endl;
      //check for KO
      removePieces(dead);
      bool is_ko = (memcmp(pieces, prev2_pieces, sizeof(pieces)) == 0);
      for (int i = 0; i < dead.numCoords; i++) {
        pieces[dead.getX(i)][dead.getY(i)] = player*-1;
      }
      if(is_ko) {
        //std::cout << "THIS IS Ko!!!!!!!!!!!!!!!!! " << std::endl;
        //ROLLBACK
        removePiece(x, y);
        memcpy(prev_pieces, temp, sizeof(prev_pieces));
        return false;
      }
    }
  } else {
    //check if placing the piece kills itself
    //If suicidal move
    if (stillAlive(newCoord)) {
      //std::cout << "CHOOSE LIFE!" << std::endl;
      //rollback
      removePiece(x, y);
      memcpy(prev_pieces, temp, sizeof(prev_pieces));
      return false;
    }
    //else was a legal move
    //updateControlMap(player, x, y);
  }
    
  //update the prevprev board
  memcpy(prev2_pieces, temp, sizeof(prev2_pieces));
  return true;
}

bool GoBoard::addPieces(coordList &toAdd, int player) {
  //add pieces back to the control board
  coordList tryDead;
  for(int piece_num = 0; piece_num < toAdd.numCoords; piece_num++) {
    coord piece = toAdd.getMove(piece_num);
    if(!placePiece(player, piece.x, piece.y, tryDead)) {
      std::cout << "ERROR ADDING PIECES";
      return false;
    }
    removePieces(tryDead);
  }
  return true;
}

bool GoBoard::removePieces(coordList &toRemove) {
  for(int piece_num = 0; piece_num < toRemove.numCoords; piece_num++) {
    coord piece = toRemove.getMove(piece_num);
    if(!removePiece(piece.x, piece.y)) {
      std::cout << "ERROR REMOVING PIECES" << std::endl;
      return false;
    }
  }
  
  //eraseControlMap();
  /*//add remaining pieces back to the control board
  for( std::map<coord,int>::iterator itr = placed_pieces.begin(); itr!=placed_pieces.end(); itr++) {
    int player = ((placed_pieces_player.find(itr->second))->second);
    coord place = itr->first;
    updateControlMap(player,place.first, place.second);
    
    //std::cout << "=======================" << std::endl; 
    //printControl();
  }*/
  return true;
}

bool GoBoard::removePiece(int x, int y) {
  //printf("Go Away: %d, %d\n", x, y);
  if(pieces[x][y] == 0) {
    return false;
  } else {
    pieces[x][y] = 0;
    /*auto piece = placed_pieces.find(coord(x,y));
    if(piece == placed_pieces.end()) {
      return false;
    }
    placed_pieces_player.erase(piece->second);
    placed_pieces.erase(piece->first);*/
    return true;
  }
}

int GoBoard::getPlayerScore(int player) {
  if(player == 1) {
    return player1_score;
  } else if(player == -1) {
    return player2_score;
  }
  std::cout << "Invalid Player" << std::endl;
  return -2;
}

//Misc
bool GoBoard::legalMove(int player, int x, int y) {
  assert(player == -1 || player == 0 || player == 1);
  if(player != 0
    && x >= 0
    && x < BOARD_SIZE
    && y >= 0
    && y < BOARD_SIZE
    && pieces[x][y] == 0) {
      return true;
    }
  return false;
}

void GoBoard::getAllReasonableMoves(int player, coordList &reasonableMoves) {
  reasonableMoves.empty();
  coordList testDead;
  
  for(int x = 0; x < BOARD_SIZE; ++x) {
    for(int y = 0; y < BOARD_SIZE; ++y) {
      bool test = placePiece(player, x, y, testDead);
      if(!test) {
        continue;
      }
      removePiece(x, y);
      if   ( (y-1 >= 0 && !isMyPiece(player, x, y-1))
          || (y+1 < BOARD_SIZE && !isMyPiece(player, x, y+1))
          || (x-1 >= 0 && !isMyPiece(player, x-1, y))
          || (x+1 < BOARD_SIZE && !isMyPiece(player, x+1, y))) {
        reasonableMoves.putMove(x, y);
        continue;
      }

      //It's an eye if it has one or zero empty adjacent diagonals (except corners)
      int count_empty_diagonals = 0;
      if (y-1 >= 0) {
        if (x-1 >= 0 && !isMyPiece(player, x-1, y-1)) count_empty_diagonals++;
        if (x+1 < BOARD_SIZE && !isMyPiece(player, x+1, y+1)) count_empty_diagonals++;
      }
      if (y+1 < BOARD_SIZE) {
        if (x-1 >= 0 && !isMyPiece(player, x-1, y-1)) count_empty_diagonals++;
        if (x+1 < BOARD_SIZE && !isMyPiece(player, x+1, y+1)) count_empty_diagonals++;
      }
      if (count_empty_diagonals > 1) {
        reasonableMoves.putMove(x, y);
      }
    }
  }
}

void GoBoard::printControl() {
  for(int j = 0; j<BOARD_SIZE; ++j) {
    for(int i = 0; i < BOARD_SIZE; ++i) {
      std::cout << control[i][j] << "\t";
    }
    std::cout << std::endl;
  }
}

void GoBoard::printBoard() {
  for(int j = 0; j<BOARD_SIZE; ++j) {
    for(int i = 0; i < BOARD_SIZE; ++i) {
      std::cout << pieces[i][j] << "\t";
    }
    std::cout << std::endl;
  }
}

void GoBoard::printPrevBoard() {
  for(int j = 0; j<BOARD_SIZE; ++j) {
    for(int i = 0; i < BOARD_SIZE; ++i) {
      std::cout << prev_pieces[i][j] << "\t";
    }
    std::cout << std::endl;
  }
}

void GoBoard::printPrev2Board() {
  for(int j = 0; j<BOARD_SIZE; ++j) {
    for(int i = 0; i < BOARD_SIZE; ++i) {
      std::cout << prev2_pieces[i][j] << "\t";
    }
    std::cout << std::endl;
  }
}

void GoBoard::printScoreBoard() {
  std::cout << "SCORE BOARD" << std::endl;
  for(int j = 0; j<BOARD_SIZE; ++j) {
    for(int i = 0; i < BOARD_SIZE; ++i) {
      std::cout << score_board[i][j] << "\t";
    }
    std::cout << std::endl;
  }
}

int GoBoard::endOfGame() {
  //until I can fix calculate scores, we'll fake it with board state evaluator
  //calculateScores();
  std::cout << "GAME OVER" << std::endl;
  int player_1_score = getBoardStateForPlayer(-1);
  int player_2_score = getBoardStateForPlayer(1);
  
  if(player_1_score > player_2_score) {
    std::cout << "Player 1 is the best." << std::endl;
    return -1;
  } else {
    std::cout << "Player 2 is the best." << std::endl;
    return 1;
  }
}

//Helper

void GoBoard::calculateScores() {
  for(auto itr = placed_pieces.begin();
              itr != placed_pieces.end();
              itr++) {
    coord next = itr->first;
    
    coord up(next.x, next.y-1);
    coord down(next.x, next.y+1);
    coord right(next.x+1, next.y);
    coord left(next.x-1, next.y);
    
    fillLine(itr->first, up);
    fillLine(itr->first, right);
    fillLine(itr->first, down);
    fillLine(itr->first, left);
  }
  
  player1_score = 0;
  player2_score = 0;
  
  for(int i = 0; i < BOARD_SIZE; ++i) {
    for(int j = 0; j < BOARD_SIZE; ++j) {
      if(score_board[i][j] <= -1) {
        player2_score += 1;
      } else if(score_board[i][j] >= 1) {
        player1_score += 1;
      }
    }
  }
  
  //printScoreBoard();
}

void GoBoard::updateControlMap(int player, int x, int y) {
  assert (player == -1 || player == 0 || player == 1);
  std::queue<waveFrontPair> waveFront;
  waveFrontType visited;

  control[x][y] += 1.0*player;
  if(fabs(control[x][y]) < 1.0) {
    control[x][y] = 1.0*player;
  }
  
  coord here(x,y);
  waveFrontPair firstEnergy = std::make_pair(here,1.0*player);
  
  waveFront.push(firstEnergy);
  visited.insert(std::pair<coord,bool>(here, true) );
  
  while(!waveFront.empty()) {
    waveFrontPair newest = waveFront.front();
    coord next = newest.first;
    waveFront.pop();
    
    double current_power = newest.second;
    double next_power = current_power/2.0;
    
    coord up(next.x, next.y-1);
    coord down(next.x, next.y+1);
    coord right(next.x+1, next.y);
    coord left(next.x-1, next.y);
    
    std::vector<coord> next_locs;
    next_locs.push_back(up);
    next_locs.push_back(down);
    next_locs.push_back(left);
    next_locs.push_back(right);
    
    for(unsigned int i=0; i< next_locs.size(); ++i) {    
      coord next_loc = next_locs[i];
      if(next_loc.x >= 0 
        && next_loc.x < BOARD_SIZE
        && next_loc.x >= 0 
        && next_loc.x < BOARD_SIZE
        && visited.find(next_loc) == visited.end()) {
        double result_power = next_power;
        //If opponents space
        if(control[next_loc.x][next_loc.y]*next_power == -1) {
          result_power += control[next_loc.x][next_loc.y];
        }
        if((placed_pieces.find(next_loc)) == placed_pieces.end()) {
          control[next_loc.x][next_loc.y] += next_power;
          
          if(fabs(result_power) > CONTROL_EPSILON) {
            waveFront.push(std::make_pair(next_loc,next_power));
            visited.insert(std::pair<coord,bool>(next_loc, true) );
          }
        }
      }
    }
  }
}

void GoBoard::updatePrevBoard(){
  for(int i = 0; i < BOARD_SIZE; ++i) {
    for(int j = 0; j < BOARD_SIZE; ++j) {
      prev_pieces[i][j] = pieces[i][j];
    }
  }
}

void GoBoard::fillLine(coord start, coord dir) {
  coord current = start;
  int player = getPlayerAtCoord(start);
  
  //std::cout << player;
  
  while(placed_pieces.find(current) == placed_pieces.end()) {
    int x = current.x;
    int y = current.y;
    
    if(x >= 0
    && x < BOARD_SIZE
    && y >= 0
    && y < BOARD_SIZE) {
      score_board[x][y] += player;
      current.x += dir.x;
      current.y += dir.y;
    } else {
      break;
    }
  }
}

bool GoBoard::isKo() {
  for(int i = 0; i < BOARD_SIZE; ++i) {
    for(int j = 0; j < BOARD_SIZE; ++j) {
      if(pieces[i][j] != prev2_pieces[i][j]) {
        return false;
      }
    }
  }
  return true;
}

void GoBoard::eraseControlMap(){
  for(int i = 0; i < BOARD_SIZE; ++i) {
    for(int j = 0; j < BOARD_SIZE; ++j) {
      control[i][j] = 0;
    }
  }
}

int GoBoard::getSurroundingDeadPieces(int player, coord last_move, coordList &chains) {
  chains.empty();
  int current_dead_count = 0;
  int visited[BOARD_SIZE][BOARD_SIZE];
  memset(visited, 0, sizeof(visited));
  
  coord adjacents[4];
  adjacents[0] = coord(last_move.x, last_move.y-1);
  adjacents[1] = coord(last_move.x, last_move.y+1);
  adjacents[2] = coord(last_move.x+1, last_move.y);
  adjacents[3] = coord(last_move.x-1, last_move.y);
  
  for (unsigned int i=0; i < 4; ++i) {
    coord next_loc = adjacents[i];
    if (next_loc.x >= 0
      && next_loc.x < BOARD_SIZE
      && next_loc.y >= 0
      && next_loc.y < BOARD_SIZE
      && pieces[next_loc.x][next_loc.y] == -player) {
      printf("looking at: %d, %d\n", next_loc.x, next_loc.y);
      current_dead_count = checkChainForDeadPieces(next_loc, visited, current_dead_count, chains);
    }
  }
  if (chains.numCoords > 0) {
    printf("You're toast: %d, %d\n", chains.getX(0), chains.getY(0));
    printf("Bounty claimed by %d, %d\n", last_move.x, last_move.y);
    printf("All %d of you\n", chains.numCoords);
  }
    
  return current_dead_count;
}

#define USE_INTS
int GoBoard::checkChainForDeadPieces(coord piece, int (&visited)[BOARD_SIZE][BOARD_SIZE], int already_visited, coordList &chain) {
  if (visited[piece.x][piece.y]) return already_visited;

  chain.putMove(piece.x, piece.y);
  visited[piece.x][piece.y] = 1;

  int num_visited = already_visited;
  int thisPlayer = getPlayerAtCoord(piece);
  bool alive = false;
  
#ifdef USE_INTS
  int adjacents[8];
  const int x = 0;
  const int y = 1;
#else
  coord adjacents[4];
#endif
  while (num_visited < chain.numCoords) {
    coord current = chain.getMove(num_visited++);
    
#ifdef USE_INTS
    adjacents[0] = current.x; adjacents[1] = current.y-1;
    adjacents[2] = current.x; adjacents[3] = current.y+1;
    adjacents[4] = current.x+1; adjacents[5] = current.y;
    adjacents[6] = current.x-1; adjacents[7] = current.y;
#else
    adjacents[0] = coord(current.x, current.y-1);
    adjacents[1] = coord(current.x, current.y+1);
    adjacents[2] = coord(current.x+1, current.y);
    adjacents[3] = coord(current.x-1, current.y);
#endif
    
    for(unsigned int i=0; i < 4; ++i) {
#ifdef USE_INTS
      if (adjacents[i*2+x] >= 0
        && adjacents[i*2+x] < BOARD_SIZE
        && adjacents[i*2+y] >= 0
        && adjacents[i*2+y] < BOARD_SIZE) {
        //If open square, the chain is still alive
        if (pieces[adjacents[i*2+x]][adjacents[i*2+y]] == 0) {
          alive = true;
        } else if (!visited[adjacents[i*2+x]][adjacents[i*2+y]] && pieces[adjacents[i*2+x]][adjacents[i*2+y]] == thisPlayer) {
          visited[adjacents[i*2+x]][adjacents[i*2+y]] = 1;
          chain.putMove(adjacents[i*2+x], adjacents[i*2+y]);
        }
#else
      coord current = adjacents[i];
      if (current.x >= 0
        && current.x < BOARD_SIZE
        && current.y >= 0
        && current.y < BOARD_SIZE) {
        //If open square, the chain is still alive
        if (pieces[current.x][current.y] == 0) {
          alive = true;
        } else if (!visited[current.x][current.y] && pieces[current.x][current.y] == thisPlayer) {
          visited[current.x][current.y] = 1;
          chain.putMove(current.x, current.y);
        }
#endif
      }
    }
  }
  if (!alive) {
    return num_visited;
  } else {
    chain.emptyTo(already_visited);
    return already_visited;
  }
}

int GoBoard::getPlayerAtCoord(coord place) {
  return pieces[place.x][place.y];
  //return(placed_pieces_player.find(placed_pieces.find(place)->second)->second);
}

std::pair<coord, int> GoBoard::getSpeculativePiece() const {
  return speculativePiece;
}

bool GoBoard::applySpeculativePiece() {
  bool success = makeMove(turn, speculativePiece.first.x, speculativePiece.first.y);
  if (success) {
    turn *= -1;
    just_passed = false;
  }
  return success;
}
void GoBoard::placeSpeculativePiece(int player, int x, int y) {
  if (player == 0) return;
  assert(player == -1 || player == 1);
  speculativePiece = std::make_pair(coord(x, y), player);
}
