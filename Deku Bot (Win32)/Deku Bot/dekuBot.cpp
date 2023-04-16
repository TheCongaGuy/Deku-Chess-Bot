#include "DekuBot.hpp"

#include <iostream>

// Explicit Constructor
DekuBot::DekuBot(GameBoard& board, const int color)
{
	currentGame = &board;
	aiColor = color;
	maxSearchTime = 0;
}

// Makes a move on the chess board
void DekuBot::MakeMove(float maxTime)
{
	// Find all possible moves
	auto moves = currentGame->FindMoves(aiColor);

	// Calculate the maximum ammount of time the AI may search for
	maxSearchTime = (int)(maxTime * 60000);

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

	int size = moves.size();

	auto maxSearchDuration = std::chrono::milliseconds(maxSearchTime);
	auto endTime = std::chrono::high_resolution_clock::now() + maxSearchDuration;

	while (std::chrono::high_resolution_clock::now() < endTime)
	{
		// Evaluate each possible move in parallel
		#pragma omp parallel for
		for (int index = 0; index < size; index++)
		{
			// Create a copy of the current game board
			GameBoard copy = *currentGame;
			// Preform the move on the copy
			copy.MovePiece(moves[index].first, moves[index].second);
			// Evaluate the result of that move
			newScore = miniMaxMove(copy, INT32_MIN, INT32_MAX, depth, endTime);

			// Stop once out of time
			if (std::chrono::high_resolution_clock::now() >= endTime)
				break;

			// Store the best move
			#pragma omp critical
			if (newScore > bestScore)
			{
				bestScore = newScore;
				bestMove.first = moves[index].first;
				bestMove.second = moves[index].second;
			}

			// Quick Return if move leads to checkmate
			if (bestScore == 1000)
			{
				std::cout << "Guaranteed Checkmate" << std::endl;
				break;
			}
		}

		// Quick Return if all moves lead to loss
		if (bestScore == -1000)
		{
			std::cout << "Checkmate - You Win" << std::endl;
			return std::pair<coordinates, coordinates>(coordinates(0, 0), coordinates(0, 0));
		}

		depth++;
	}

	// For fun, calculate confidence of move
	bestScore += 226;
	bestScore *= 100;
	float confidence = bestScore / 400.f;
	std::cout << "Confidence: " << std::setprecision(4) << confidence << "%" << std::endl;

	return bestMove;
}

// Recursively find the best possible outcome for a move
// Returns an integer
int DekuBot::miniMaxMove(GameBoard& nextGame, int alpha, int beta, int currentDepth, std::chrono::steady_clock::time_point endTime)
{
	// Calculate fitness of current board
	int fitness = nextGame.RankBoard(aiColor);

	// Return if checkmate was reached
	if (fitness >= 1000 || fitness <= -1000)
		return fitness;

	// Return Leaf Node
	if (currentDepth <= 0)
		return fitness;

	// Return invalid if search time was reached
	if (std::chrono::high_resolution_clock::now() >= endTime)
		return -1000;

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