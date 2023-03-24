#include "DekuBot.hpp"

#include <iostream>

// Explicit Constructor
DekuBot::DekuBot(GameBoard* board, const int color)
{
	currentGame = board;
	aiColor = color;
	maxSearchTime = 0;
}

// Makes a move on the chess board
void DekuBot::MakeMove(int maxTime)
{
	// Find all possible moves
	auto moves = currentGame->FindMoves(aiColor);

	// Calculate the maximum ammount of time the AI may search for
	maxSearchTime = (maxTime * 60000) / moves.size();

	// Best Move
	std::pair<coordinates, coordinates> bestMove;
	int bestScore = INT32_MIN;
	int newScore = 0;

	// Evaluate each possible move
	for (auto& move : moves)
	{
		// Create a copy of the current game board
		GameBoard copy = *currentGame;
		// Preform the move on the copy
		copy.MovePiece(move.first, move.second);
		// Evaluate the result of that move
		clock_t startTime = clock();
		newScore = miniMaxMove(copy, INT32_MIN, INT32_MAX, 0, startTime);

		// Store the best move
		if (newScore >= bestScore)
		{
			bestScore = newScore;
			bestMove.first = move.first;
			bestMove.second = move.second;
		}
	}

	// Preform the best move
	currentGame->MovePiece(bestMove.first, bestMove.second);
}

// Recursively find the best possible outcome for a move
// Returns an integer
int DekuBot::miniMaxMove(GameBoard& nextGame, int alpha, int beta, int currentDepth, clock_t startTime)
{
	// Calculate fitness of current board
	int fitness = nextGame.RankBoard(aiColor);

	// Check that maximum time has not been reached
	if (difftime(clock(), startTime) >= maxSearchTime)
		return fitness;


	// Cut Search Early at depth of 1000 (Maximum Stack Depth for Windows)
	if (currentDepth >= 1000)
		return fitness;

	// Return in event of checkMate
	if (fitness == 1000 || fitness == -1000)
		return fitness;

	// Find best move if it is AI's turn
	if (aiColor == 1 && nextGame.whosTurn() || aiColor == -1 && !nextGame.whosTurn())
	{
		int maxValue = INT32_MIN;

		for (auto& move : nextGame.FindMoves(aiColor))
		{
			GameBoard copy = nextGame;

			copy.MovePiece(move.first, move.second);
			int newValue = miniMaxMove(copy, alpha, beta, currentDepth + 1, startTime);
			
			if (newValue > maxValue)
				maxValue = newValue;

			if (maxValue > alpha)
				alpha = maxValue;

			if (beta <= alpha)
				break;
		}

		return maxValue;
	}

	// Find worst move if it is player's turn
	else
	{
		int minValue = INT32_MAX;

		for (auto& move : nextGame.FindMoves(-aiColor))
		{
			GameBoard copy = nextGame;
			copy.MovePiece(move.first, move.second);

			int newValue = miniMaxMove(copy, alpha, beta, currentDepth + 1, startTime);

			if (newValue < minValue)
				minValue = newValue;

			if (minValue < beta)
				beta = minValue;

			if (beta <= alpha)
				break;
		}

		return minValue;
	}
}