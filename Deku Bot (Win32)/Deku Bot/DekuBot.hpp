#pragma once

#include <chrono>
#include <iomanip>
#include <thread>
#include <omp.h>

#include "GameBoard.hpp"

// Deku Chess Bot
class DekuBot
{
public:
	// Explicit Constructor takes a reference to an existing game board and the AI's color (1 -> White | -1 -> Black)
	DekuBot(GameBoard& board, const int color);

	// Makes a move on the chess board
	// Takes the maximum ammount of time in minutes the AI is allowed to search
	void MakeMove(float maxTime);

private:
	// ----- Data Members ----- \\

	// Pointer to the live game board
	GameBoard* currentGame;

	// Color assigned to the AI
	// (1 -> White | -1 -> Black)
	int aiColor;

	// Maximum ammount of time specified by the user for each move in milliseconds
	int maxSearchTime;

	// ----- Methods ----- \\

	// Runs a minimax algorithm using iterative deepening
	// Takes a vector of moves (pair of coordinates)
	// Returns the best move after a given amount of time (pair of coordinates)
	std::pair<coordinates, coordinates> breadthFirstSearch(std::vector<std::pair<coordinates, coordinates>>& moves);

	// Recursively find the best possible outcome for a move
	// Takes a game board state, an alpha and beta value, the current depth of search, and the specified time to stop searching
	// Returns an integer representing the fitness of that move
	int miniMaxMove(GameBoard& nextGame, int alpha, int beta, int currentDepth, std::chrono::steady_clock::time_point endTime);
};