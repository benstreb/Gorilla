#include "glCanvas.h"
#include <iostream> 
#include <ctime> 
#include <cstdlib>

#include "argparser.h"
#include "mesh.h"

// =========================================
// =========================================

int main(int argc, char *argv[]) {
  ArgParser *args = new ArgParser(argc, argv);
  if(args->gui) {
    Mesh *mesh = new Mesh(args);
    //int i = mesh->LoadBoard("NBoard.obj", 0);
    //mesh->LoadPiece("NPiece.obj", i);
    int i = mesh->LoadBoard("Cube.obj", 0);
    i = mesh->LoadPiece("Sphere.obj", i);
    mesh->LoadRect("Cube.obj", i);
    mesh->ComputeGouraudNormals();

    glutInit(&argc,argv);
    GLCanvas::initialize(args,mesh);

    delete mesh;
  } else {
    std::cout << "Command Prompt Version" << std::endl;
    GoBoard* theBoard = new GoBoard();
    
    bool game_over = false;
    while(!game_over) {
      //AI move 1
      coord ai1_move = args->theAI->getMove(theBoard);
      if(ai1_move == coord(-1,-1)) {
        if(theBoard->getJustPassed()) {
          theBoard->endOfGame();
          game_over = true;
        } else {
          theBoard->passTurn();
        }
      } else {
        theBoard->makeMove(theBoard->getTurn(), ai1_move.x, ai1_move.y);
        theBoard->nextTurn();
      }
    
      coord ai2_move = args->theAI_2->getMove(theBoard);
      if(ai2_move == coord(-1,-1)) {
        if(theBoard->getJustPassed()) {
          theBoard->endOfGame();
          game_over = true;
        } else {
          theBoard->passTurn();
        }
      } else {
        theBoard->makeMove(theBoard->getTurn(), ai2_move.x, ai2_move.y);
        theBoard->nextTurn();
      }  
    }
    std::cout << "resulting board: " << std::endl;
    theBoard->printBoard();
    delete theBoard;
    printf("Press enter to quit");
    getc(stdin);
  }

  delete args;
  return 0;
}

// =========================================
// =========================================
