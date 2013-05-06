Gorilla
Authors: Andrew Wright and Benjamin Streb 
Spring 2013
=======
Welcome to Gorilla, a Go playing program.
If you have issues running the GUI, there
is a command prompt version available
for AI vs AI combat.

Compiling Instructions
======================
This program is compiled using cmake.
Once you have cmake installed, open
the command prompt, change working
directory to this folder and enter:
cmake ./

Afterwards type
make

You should then be able to run the program
by typing ./go followed by your desired arguments.

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


==============================================================
Algorithms In depth
==============================================================
A: Go-rules enforcement
	A1: Piece Capture
	A2: Suicide Prevention
	A3: Ko Rule
	A4: scoring

B: Board state evaluation
	B1: Control Calculation
	B2: Position evaluation

C: AI
	1: Monte-Carlo
==============================================================
There are three major categories of algorithms used
in Gorilla. They are as follows:

A-Go rules enforcement
B-Board state evaluation
C-AI

A: Go-rules enforcement
=============================================
-In Gorilla we include rules enforcement for:
1: Piece Capture
2: Suicide Prevention
3: Ko
4: Scoring

We did NOT check to prevent triple KO.

A1: Piece Capture
===========================
-Whenever a piece is placed we check the up to
4 adjacent spaces. We use these spaces as a start
point for a chain detection algorithm. 

The chain detection uses a map to prevent visitng
the same space twice, a queue to keep track
of unexpanded spaces, and a vector to track stones
in the chain. 

At the start, one of those 4 adjacent spaces is
passed into the queue and vector. Then the queue 
loops until it is empty. In each iteration we pop 
off the head of the queue and look at the 4 adjacent 
spaces to the popped off element.

If one of those spaces is an open space, the loop breaks
and we have determined that this chain is alive.

Any of those spaces that have stones belonging to the 
opponent are pushed into the queue, and the vector.

If the queue empties, it means the chain has no 
remaining liberties, and all stones in this chain
are removed.

A2: Suicide Prevention
==============================
In Go, a move which would cause any of your own pieces to immediatly
die is an illegal move. However, if your piece would take an
opponent's pieces last liberty it may be placed in a space that
would otherwise be illegal to place it in.

We enforce this rule by checking the most recently placed stone
if it was unable to capture any enemy stones. We apply the chain
detection algorithm to it alone instead of the 4 adjacent spaces.
If it is included in a dead chain then this move is illegal, 
and changes to the board are rolled back.

A3: Ko
===============================
In order to prevent draws, and games from going on forever
the rule of Ko was introduced. In Go there are board states
that without this rule leave both players with the same move
every turn.

Example: 
 1          2            3
-WB-	  -WB-         -WB-
W  B ---> W-WB ------> WB-B 
-WB-	  -WB-         -WB-

As you can see, white could then capture black's stone back
and the board would be back to state 2. Then black could 
capture back, and return to stae 3 ect. 

In Go it is ilegal to make a move that would cause the board
to be in the same state as your previous turn ended. This is
the rule of Ko.

To enforce this rule we keep track of the past 2 board states,
update them as moves are made, and on turns in which single
stones are captured we compare the board state to the one
from 2 turns ago. If they are equal we rollback the move.

Scoring
=================================
Scoring is a hard problem in Go, since scores are usually
agreed upon by the players, and the definition is the
ambiguously defined "terriority surrounded by your pieces"

We do have a scoring system, but it is sort of a hack
that works on full boards. We re-use our board state evalution
code that will be explained later. Basically, it assigns
a control value to each space, and we sum these up for each
player to determine a score.


B: Board state evaluation
=================================
-In order to write any form of reasonable AI we needed
to implement a system by which the AI could judge how
strong or weak of a position it was in at any given board
state. Since the goal of Go is to surround as much of the
board as possible we wanted our board state evaluation
to be an attempt at quantifying control.

1: The control map
=====================
We quantified control as a float value between -1 and 1
with -1 being completely under black's control and 1
being completely under white's control. The control value
for each space was stored in a 2D array mirroring that
of the 2D array that tracks pieces placed on the board.

Control was calculated as follows:
Whenever a piece was placed on the board, it sent out
a propogation. The propogation was managed with 2
data structures: a queue representing the wave front,
and a map to prevent revisiting of squares. 

Initially, the control value of the newly placed piece's
square was set to 1 or -1 depending on the player. It
was then pushed into the queue and map.

Then the algorithm looped until the queue was empty.
At each iteration it sent a propogation to each adjacent
space that didn't contain an enemy stone. The power
of the propogation was added against the control currently
on that space. The propogation was then weakened if that
space was under enemy control. If it had any power left,
then this new propogation was added to the queue.

This allowed multiple ally stones in an area to grant
high control while properly depicting spaces between
battle lines as belonging to neither player.

The control is calculate incrementally until a piece
is removed from the board. At which board the control map
is reset, and the remaining pieces are added back to the 
board.

2: Position evaluation
========================
With the control map calculated, a player's board position
is evaluated to be the sum of all their control minus the
sum of the opponent's control. We make sure to subtract
out the opponent's score to deal with imaginable situations
in which both players have high scores, but the opponent
actually has more than you.


C: AI
========================
I implemented 2 AIs. The first is simply an AI that makes random
moves. The next is an AI that uses Monte-Carlo sampling. 

Both AIs use an algorithm that determines all "reasonable moves".
This algorithm returns a collection of all legal moves excluding
those which fill in single point eyes. Those moves are, except
in rare circumstances, bad as they do nothing but reduce
the amount of liberties available to your pieces.

1: Monte-Carlo AI
========================
I implemented an extremely straight forward Monte-Carlo AI.
You supply to it 2 integers G and T. 

Every turn it evaluates each of the available "reasonable" moves.
For each of those moves it makes a test board and makes the
reasonable move. It then simulates T turns of a game in which
both players proceed to make random reasonable moves. At the 
end of those turns in runs the position evaluation. It adds
the result of this to a total. This process is repeated a total of G
times, and the totals are averaged at the end. This average is
used as an estimation of the value of making this move. 

If this value is higher than any seen yet this turn, it is moved
to the new max, and the current choice is changed to this move.

After going through all the reasonable moves, the one with the
highest average board position is chosen. 


Results and Reflections
==============================================================
I ran the following experiments:
1: Random AI versus Monte-Carlo
2: Monte-Carlo vs Monte-Carlo
3: Monte-Carlo versus Novice Humans with moderate Game and turn count

I did a large number of variations on experiment 1, by trying differing
values for G and T. Here were my findings.

1: Even for the lowest values for G and T, the monte-carlo algorithm
outpreforms random AI minimum 10:1. This isn't too interesting of a result
but it is good to know that our AI is better than just randomly picking moves.

2: The differing results for including a higher T versus G was interesting.
Upon watching about 50 games, AI running a high T value tended to make moves that
grabbed more space, but rarely acted to defend threatened chains, even those
with a significant number of stones. This is a result of the random nature of the
monte carlo sampling. If the opponent during sampling doesnt randomly capture the stones
before the AI can provide them with more liberty, it will be oblivious to the danger
of losing them. 

The AI with a higher count for G was able to avoid being captured in the short term,
but often wasn't as aggressive for territory grab. However, against human players 
I hypothesize that you can get more bang for you CPU time buck by increasing G more than T. I would 
like to research this more closely at a later point. 

3: Novice humans struggled with moderate settings for G and T but usually came
out ahead in the end. The AI tends to perform well on the large scale, but is
dominated in capture wars that require more focused processing. I would like
to experiment with higer values, but as it stand, the calculation time for
moves is prohibitive.


Overall I'm pretty satisfied by our current progress. The most pressing matter is to try
to increase the algorithm efficiciency for the Monte Carlo sampling by improving the
speed of our rules detection, and researching various hueristics. As it stands we've
already made a few passes at optmization which yielded great gains, but it will be
a challenge to reach run-times condusive for large scale tests. 
















