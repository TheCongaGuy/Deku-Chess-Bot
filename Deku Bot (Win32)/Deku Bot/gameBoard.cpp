#include "GameBoard.hpp"

GameBoard::GameBoard()
{
	// Set default values
	movesSinceCapture = 0;
	blackInCheck = false;
	whiteInCheck = false;

	// Configuration of back ranks
	piece backRanks[8] = { 4, 5, 6, 7, 9, 6, 5, 4 };

	for (int x = 0; x < 8; x++)
	{
		// Set front ranks for each color
		gameBoard[x][1] = -1;
		gameBoard[x][6] = 1;

		// Set back ranks for each color
		gameBoard[x][0] = -backRanks[x];
		gameBoard[x][7] = backRanks[x];
	}
}