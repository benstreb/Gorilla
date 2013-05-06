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
  if(args->gui)
  {
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
  }
  else
  {
	  std::cout << "Command Prompt Version" << std::endl;
	  GoBoard* theBoard = new GoBoard();
	  
	  bool game_over = false;
	  while(!game_over)
	  {
		//AI move 1
		coord ai1_move = args->theAI->getMove(theBoard);
		if(ai1_move == std::make_pair(-1,-1))
		{
			if(theBoard->getJustPassed())
			{
				theBoard->endOfGame();
				game_over = true;
			}
			else
			{
				theBoard->passTurn();
			}
		}
		else
		{
			theBoard->placePiece(theBoard->getTurn(), ai1_move.first, ai1_move.second);
			theBoard->nextTurn();
		}
		
		coord ai2_move = args->theAI_2->getMove(theBoard);
		if(ai2_move == std::make_pair(-1,-1))
		{
			if(theBoard->getJustPassed())
			{
				theBoard->endOfGame();
				game_over = true;
			}
			else
			{
				theBoard->passTurn();
			}
		}
		else
		{
			theBoard->placePiece(theBoard->getTurn(), ai2_move.first, ai2_move.second);
			theBoard->nextTurn();
		}	
	  }
	std::cout << "resulting board: " << std::endl;
	theBoard->printBoard();
	delete theBoard;
  }

  
  delete args;
  return 0;
}

// =========================================
// =========================================
