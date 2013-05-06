Gorilla
=======

USAGE
============
exe name: go

Arguments	Result
none 		opens a player vs player game with the 3D GUI

-no_gui		opens a command prompt display meant for ai vs ai

-eval		-allows the monte carlo move evaluator function. The 			following two arguments must be integers. The first 			determines how many sample games to run per move.
	  	The second determines how many turns per sample game to 		play.

-ai_vs_ai	game will be ai vs ai mode. Next 2 arguments must be valid 			ai names 

-ai_random	an AI that chooses 1 random move from a list of legal moves
	 	excluding moves that would fill in its own eyes

-ai_monte_carlo	an AI that uses random sampling to choose the best move.
	   	The following two arguments must be integers. The 			first determines how many sample games to run per move.
	  	The second determines how many turns per sample game to 		play. 

Example usages:
./go
./go -ai_random
./go -ai_vs_ai -ai_random -ai_random
./go -no_gui -ai_vs_ai -ai_random -ai_monte_carlo 5 5
./go -eval 10 3

USING THE GUI
======================================
General controls
================
Middle-click and drag: translate board

c: view board control visualization
r: reset the board

Specific controls
================
Player vs. Player
Player vs. AI
-----------------
Press left mouse button to select a move.
Right click to pass turn.

AI vs. AI
-----------------
Left click to view the AI players next moves

evaluator
-----------------
If the evaluator argument is set, press e to evaluate a move


======================================
Go player to be.
Right now it only visualizes a fixed position that is designed for ease of
procedural generation rather than plausibility. We should have something
that is almost shadow maps very soon though.
