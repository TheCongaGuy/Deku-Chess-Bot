#pragma once

#include "GameBoard.hpp"
#include <time.h>

// Deku Chess Bot
class DekuBot
{
public:
	// Explicit Constructor takes a reference to an existing game board and the AI's color (1 -> White | -1 -> Black)
	DekuBot(GameBoard* board, const int color);

	// Makes a move on the chess board
	// Takes the maximum ammount of time in minutes the AI is allowed to search
	void MakeMove(int maxTime);

private:
	// ----- Data Members ----- \\

	// Reference to a game board
	GameBoard* currentGame;

	// Color assigned to the AI
	// (1 -> White | -1 -> Black)
	int aiColor;

	// Maximum ammount of time specified by the user for each move in milliseconds
	int maxSearchTime;

	// ----- Methods ----- \\

	// Recursively find the best possible outcome for a move
	// Returns an integer
	int miniMaxMove(GameBoard& nextGame, int alpha, int beta, int currentDepth, clock_t startTime);
};