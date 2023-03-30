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
	maxSearchTime = maxTime * 60000;

	// Best Move
	std::pair<coordinates, coordinates> bestMove = breadthFirstSearch(moves);

	// Preform the best move
	currentGame->MovePiece(bestMove.first, bestMove.second);
}

// Search the tree Breadth First
// Returns the best move after a given amount of time
std::pair<coordinates, coordinates> DekuBot::breadthFirstSearch(std::vector<std::pair<coordinates, coordinates>>& moves)
{
	std::pair<coordinates, coordinates> bestMove;
	bestMove.first = coordinates(-1, -1);
	bestMove.second = coordinates(-1, -1);
	int bestScore = INT32_MIN;
	int newScore = 0;
	int depth = 1;

	auto maxSearchDuration = std::chrono::milliseconds(maxSearchTime);
    auto endTime = std::chrono::high_resolution_clock::now() + maxSearchDuration;

    while (std::chrono::high_resolution_clock::now() < endTime)
	{
		// Evaluate each possible move
		for (auto& move : moves)
		{
			// Create a copy of the current game board
			GameBoard copy = *currentGame;
			// Preform the move on the copy
			copy.MovePiece(move.first, move.second);
			// Evaluate the result of that move
			newScore = miniMaxMove(copy, INT32_MIN, INT32_MAX, depth, endTime);

			// Store the best move
			if (newScore > bestScore)
			{
				bestScore = newScore;
				bestMove.first = move.first;
				bestMove.second = move.second;
			}
		}

		depth++;
	}
	
	// For fun, calculate confidence of move
	bestScore += 1000;
	bestScore *= 100;
	float confidence = bestScore / 2000.f;
	std::cout << "Confidence: " << confidence << "%" << std::endl;

	return bestMove;
}

// Recursively find the best possible outcome for a move
// Returns an integer
int DekuBot::miniMaxMove(GameBoard& nextGame, int alpha, int beta, int currentDepth, std::chrono::_V2::system_clock::time_point endTime)
{
	// Calculate fitness of current board
	int fitness = nextGame.RankBoard(aiColor);

	// Bias fitness based on depth of search
	if (aiColor == 1 && nextGame.whosTurn() || aiColor == -1 && !nextGame.whosTurn())
		fitness += currentDepth;
	fitness -= currentDepth;

	// Return Leaf Node
	if (currentDepth <= 0)
		return fitness;

	// Return invalid if search time was reached
	if (std::chrono::high_resolution_clock::now() >= endTime)
	{
		if (aiColor == 1 && nextGame.whosTurn() || aiColor == -1 && !nextGame.whosTurn())
			return INT32_MAX;

		return INT32_MIN;
	}

	// Return if checkmate was reached
	if (fitness >= 1000 || fitness <= -1000)
		return fitness;


	// Find best move if it is AI's turn
	if (aiColor == 1 && nextGame.whosTurn() || aiColor == -1 && !nextGame.whosTurn())
	{
		int maxValue = INT32_MIN;

		for (auto& move : nextGame.FindMoves(aiColor))
		{
			GameBoard copy = nextGame;

			copy.MovePiece(move.first, move.second);
			int newValue = miniMaxMove(copy, alpha, beta, currentDepth - 1, endTime);
			
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

			int newValue = miniMaxMove(copy, alpha, beta, currentDepth - 1, endTime);

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