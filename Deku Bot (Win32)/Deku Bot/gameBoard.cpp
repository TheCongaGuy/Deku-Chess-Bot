#include "GameBoard.hpp"

// Default Constructor
GameBoard::GameBoard()
{
	// Set flags
	movesSinceCapture = 0;
	blackInCheck = whiteInCheck = false;
	blackPieces = whitePieces = 16;

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

	// Add the starting configuration to the moves map
	previousPositions.emplace(std::pair<GameBoard, int>(*this, 1));
}

// Explicit Constructor
// Takes a reference to a pre-made game board
GameBoard::GameBoard(const piece(&gameState)[8][8])
{
	// Copy the piece positions to this objects game board
	for (int x = 0; x < 8; x++)
		for (int y = 0; y < 8; y++)
			gameBoard[x][y] = gameState[x][y];

	// Evaluate the game board position
	EvaluateBoard();

	// Assume a capture was just made
	movesSinceCapture = 0;

	// Add the position configuration to the moves map
	previousPositions.emplace(std::pair<GameBoard, int>(*this, 1));
}

// Copy Constructor
GameBoard::GameBoard(const GameBoard& rhs)
{
	// Copy flags
	movesSinceCapture = rhs.movesSinceCapture;
	blackInCheck = rhs.blackInCheck;
	whiteInCheck = rhs.whiteInCheck;
	blackPieces = rhs.blackPieces;
	whitePieces = rhs.whitePieces;

	// Copy the board of the other object
	for (int x = 0; x < 8; x++)
		for (int y = 0; y < 8; y++)
			gameBoard[x][y] = rhs.gameBoard[x][y];

	// Copy map of previous positions
	for (auto& position : rhs.previousPositions)
		previousPositions.emplace(position);
}

// Evaluates the current board and updates pieces / flags
void GameBoard::EvaluateBoard()
{
	// Flag for pawn movement
	bool pawnMove = false;
	// Holds coordinates of kings
	int blackKingX = -1, blackKingY = -1, whiteKingX = -1, whiteKingY = -1;
	// Holds number of pieces on the board
	int currentBlackPieces = 0, currentWhitePieces = 0;

	// Check state of the board
	for (int x = 0; x < 8; x++)
	{
		// Check Past En Passant Opportunities
		if (gameBoard[x][3] == -2)
		{
			if (gameBoard[x][2] == 1)
				gameBoard[x][3] = 0;
			else
				gameBoard[x][3] = -1;
		}

		if (gameBoard[x][4] == 2)
		{
			if (gameBoard[x][5] == -1)
				gameBoard[x][4] = 0;
			else
				gameBoard[x][5] = 1;
		}

		// Check New En Passant Opportunities
		if (gameBoard[x][4] == 1)
			if (previousPositions.size() && previousPositions.end()->first.gameBoard[x][6] == 1)
			{
				gameBoard[x][4] = 2;
				pawnMove = true;
			}

		if (gameBoard[x][3] == -1)
			if (previousPositions.size() && previousPositions.end()->first.gameBoard[x][1] == -1)
			{
				gameBoard[x][3] = -2;
				pawnMove = true;
			}

		// Check Promotions
		if (gameBoard[x][7] == -1)
		{
			gameBoard[x][7] = -7;
			pawnMove = true;
		}

		if (gameBoard[x][0] == 1)
		{
			gameBoard[x][1] = 7;
			pawnMove = true;
		}

		for (int y = 0; y < 8; y++)
		{
			// Count black pieces
			if (gameBoard[x][y] < 0)
				currentBlackPieces++;

			// Count white pieces
			if (gameBoard[x][y] > 0)
				currentWhitePieces++;

			// Check pawn movement
			if (gameBoard[x][y] == 1 && previousPositions.size() && previousPositions.end()->first.gameBoard[x][y] == 0)
				pawnMove = true;

			if (gameBoard[x][y] == -1 && previousPositions.size() && previousPositions.end()->first.gameBoard[x][y] == 0)
				pawnMove = true;

			// Check Castle Rooks
			if (x % 7 != 0 && y % 7 != 0)
			{
				if (gameBoard[x][y] == 4)
					gameBoard[x][y] = 3;

				if (gameBoard[x][y] == -4)
					gameBoard[x][y] = -3;
			}

			// Check Kings
			if (gameBoard[x][y] == 8 || gameBoard[x][y] == 9)
			{
				// Record king location
				whiteKingX = x;
				whiteKingY = y;

				// Check if king moved from start
				if (x != 4 && y != 7)
					gameBoard[x][y] = 8;
			}

			if (gameBoard[x][y] == -8 || gameBoard[x][y] == -9)
			{
				// Record king location
				blackKingX = x;
				blackKingY = y;

				// Check if king moved from start
				if (x != 4 && y != 0)
					gameBoard[x][y] = 8;
			}
		}
	}

	// Update flags
	if (pawnMove || blackPieces > currentBlackPieces || whitePieces > currentWhitePieces)
		movesSinceCapture = 0;
	else
		movesSinceCapture++;

	blackPieces = currentBlackPieces;
	whitePieces = currentWhitePieces;

	// Check for draws
	if (movesSinceCapture == 100)
	{
		blackInCheck = whiteInCheck = true;
		return;
	}

	for (auto& position : previousPositions)
		if (position.second >= 3)
		{
			blackInCheck = whiteInCheck = true;
			return;
		}

	// [TODO: CALCULATE TOTAL MOVES METHOD]
	auto results = FindMoves(-1);
	if (results.size() == 0 && !blackInCheck)
	{
		blackInCheck = whiteInCheck = true;
		return;
	}

	results = FindMoves(1);
	if (results.size() == 0 && !whiteInCheck)
	{
		blackInCheck = whiteInCheck = true;
		return;
	}

	// Check for king existance
	if (blackKingX == -1 || whiteKingX == -1)
		return;

	// Check for pawn checks
	if (gameBoard[blackKingX + 1][blackKingY + 1] == 1 || gameBoard[blackKingX + 1][blackKingY + 1] == 2)
		blackInCheck = true;
	if (gameBoard[blackKingX - 1][blackKingY + 1] == 1 || gameBoard[blackKingX - 1][blackKingY + 1] == 2)
		blackInCheck = true;

	if (gameBoard[whiteKingX + 1][whiteKingY - 1] == -1 || gameBoard[whiteKingX + 1][whiteKingY - 1] == -2)
		whiteInCheck = true;
	if (gameBoard[whiteKingX - 1][whiteKingY - 1] == -1 || gameBoard[whiteKingX - 1][whiteKingY - 1] == -2)
		whiteInCheck = true;

	// Check for knight checks
	for (int areaX = -2; areaX <= 2; areaX++)
		for (int areaY = -2; areaY <= 2; areaY++)
		{
			int distance = abs(areaX) + abs(areaY);
			if (distance == 3)
			{
				int newX = blackKingX + areaX;
				int newY = blackKingY + areaY;

				if (InBounds(newX, newY) && gameBoard[newX][newY] == 5)
				{
					blackInCheck = true;
					return;
				}

				newX = whiteKingX + areaX;
				newY = whiteKingY + areaY;
				if (InBounds(newX, newY) && gameBoard[newX][newY] == -5)
				{
					whiteInCheck = true;
					return;
				}
			}
		}

	// Check for diagonal checks
	int directionX[4] = { 1, 1, -1, -1 };
	int directionY[4] = { 1, -1, 1, -1 };
	for (int i = 0; i < 4; i++)
	{
		int newX = blackKingX + directionX[i];
		int newY = blackKingX + directionY[i];
		while (InBounds(newX, newY))
		{
			// Friendly piece blocks path
			if (gameBoard[newX][newY] < 0)
				break;

			// Bishop or Queen creates check
			if (gameBoard[newX][newY] == 6 || gameBoard[newX][newY] == 7)
				blackInCheck = true;

			newX += directionX[i];
			newY += directionY[i];
		}
	}

	for (int i = 0; i < 4; i++)
	{
		int directionX[4] = { 1, 1, -1, -1 };
		int directionY[4] = { 1, -1, 1, -1 };
		int newX = whiteKingX + directionX[i];
		int newY = whiteKingX + directionY[i];
		while (InBounds(newX, newY))
		{
			// Friendly piece blocks path
			if (gameBoard[newX][newY] > 0)
				break;

			// Bishop or Queen creates check
			if (gameBoard[newX][newY] == -6 || gameBoard[newX][newY] == -7)
				blackInCheck = true;

			newX += directionX[i];
			newY += directionY[i];
		}
	}

	// Check for coordinal checks
	for (int i = 0; i < 4; i++)
	{
		int directionX[4] = { 0, 1, 0, -1 };
		int directionY[4] = { 1, 0, -1, 0 };
		int newX = blackKingX + directionX[i];
		int newY = blackKingY + directionY[i];
		while (InBounds(newX, newY))
		{
			// Friendly piece blocks path
			if (gameBoard[newX][newY] < 0)
				break;

			// Rook or Queen creates check
			if (gameBoard[newX][newY] == 3 || gameBoard[newX][newY] == 4 || gameBoard[newX][newY] == 7)
				blackInCheck = true;

			newX += directionX[i];
			newY += directionY[i];
		}
	}

	for (int i = 0; i < 4; i++)
	{
		int directionX[4] = { 0, 1, 0, -1 };
		int directionY[4] = { 1, 0, -1, 0 };
		int newX = whiteKingX + directionX[i];
		int newY = whiteKingY + directionY[i];
		while (InBounds(newX, newY))
		{
			// Friendly piece blocks path
			if (gameBoard[newX][newY] > 0)
				break;

			// Rook or Queen creates check
			if (gameBoard[newX][newY] == -3 || gameBoard[newX][newY] == -4 || gameBoard[newX][newY] == -7)
				blackInCheck = true;

			newX += directionX[i];
			newY += directionY[i];
		}
	}
}

// Finds all possible moves for a given color (1 for white, -1 for black)
// Returns a vector of pairs of coordinates (pair<int, int>)
// pair.first -> initial position | pair.second -> final position
std::vector<std::pair<coordinates, coordinates>> GameBoard::FindMoves(int color) const
{
	// Vector of possible moves
	std::vector<std::pair<coordinates, coordinates>> possibleMoves;

	// Check each grid space
	for (int x = 0; x < 8; x++)
		for (int y = 0; y < 8; y++)
			switch (gameBoard[x][y] * color)
			{
				// Pawns
				case 1:
					// For black pawns
					if (color == -1)
					{
						// Check for En Passant
						if (y == 1 && gameBoard[x][2] == 0 && gameBoard[x][3] == 0)
						{
							coordinates initial(x, y);
							coordinates final(x, 3);
							std::pair<coordinates, coordinates> move(initial, final);
							possibleMoves.emplace_back(move);
						}
					}

					// For white pawns
					if (color == 1)
					{
						// Check for En Passant
						if (y == 6 && gameBoard[x][5] == 0 && gameBoard[x][4] == 0)
						{
							coordinates initial(x, y);
							coordinates final(x, 4);
							std::pair<coordinates, coordinates> move(initial, final);
							possibleMoves.emplace_back(move);
						}
					}
				case 2:
					// For black pawns
					if (color == -1)
					{
						// Check space infront
						if (InBounds(x, y + 1) && gameBoard[x][y + 1] == 0)
						{
							coordinates initial(x, y);
							coordinates final(x, y + 1);
							std::pair<coordinates, coordinates> move(initial, final);
							possibleMoves.emplace_back(move);
						}

						// Check diagonals
						if (InBounds(x + 1, y + 1) && (gameBoard[x + 1][y + 1] > 0 || gameBoard[x + 1][y] == 2))
						{
							coordinates initial(x, y);
							coordinates final(x + 1, y + 1);
							std::pair<coordinates, coordinates> move(initial, final);
							possibleMoves.emplace_back(move);
						}

						if (InBounds(x - 1, y + 1) && (gameBoard[x - 1][y + 1] > 0 || gameBoard[x - 1][y] == 2))
						{
							coordinates initial(x, y);
							coordinates final(x - 1, y + 1);
							std::pair<coordinates, coordinates> move(initial, final);
							possibleMoves.emplace_back(move);
						}
					}

					// For white pawns
					if (color == 1)
					{
						// Check space infront
						if (InBounds(x, y - 1) && gameBoard[x][y - 1] == 0)
						{
							coordinates initial(x, y);
							coordinates final(x, y - 1);
							std::pair<coordinates, coordinates> move(initial, final);
							possibleMoves.emplace_back(move);
						}

						// Check diagonals
						if (InBounds(x + 1, y - 1) && (gameBoard[x + 1][y - 1] > 0 || gameBoard[x + 1][y] == 2))
						{
							coordinates initial(x, y);
							coordinates final(x + 1, y - 1);
							std::pair<coordinates, coordinates> move(initial, final);
							possibleMoves.emplace_back(move);
						}

						if (InBounds(x - 1, y - 1) && (gameBoard[x - 1][y - 1] > 0 || gameBoard[x - 1][y] == 2))
						{
							coordinates initial(x, y);
							coordinates final(x - 1, y - 1);
							std::pair<coordinates, coordinates> move(initial, final);
							possibleMoves.emplace_back(move);
						}
					}
					break;

				// Rooks
				case 3:
				case 4:
					// Check all four directions
					for (int i = 0; i < 4; i++)
					{
						int directionX[4] = { 0, 1, 0, -1 };
						int directionY[4] = { 1, 0, -1, 0 };
						int newX = x + directionX[i];
						int newY = y + directionY[i];
						while (InBounds(newX, newY) && gameBoard[newX][newY] == 0)
						{
							coordinates initial(x, y);
							coordinates final(newX, newY);
							std::pair<coordinates, coordinates> move(initial, final);
							possibleMoves.emplace_back(move);

							newX += directionX[i];
							newY += directionY[i];
						}
					}
					break;

				// Knights
				case 5:
					for (int areaX = -2; areaX <= 2; areaX++)
						for (int areaY = -2; areaY <= 2; areaY++)
						{
							int distance = abs(areaX) + abs(areaY);
							if (distance == 3)
							{
								int newX = x + areaX;
								int newY = y + areaY;
								if (InBounds(newX, newY) && gameBoard[newX][newY] == 0)
								{
									coordinates initial(x, y);
									coordinates final(newX, newY);
									std::pair<coordinates, coordinates> move(initial, final);
									possibleMoves.emplace_back(move);
								}
							}
						}
					break;

				// Bishops
				case 6:
					// Check all four directions
					for (int i = 0; i < 4; i++)
					{
						int directionX[4] = { 1, 1, -1, -1 };
						int directionY[4] = { 1, -1, 1, -1 };
						int newX = x + directionX[i];
						int newY = y + directionY[i];
						while (InBounds(newX, newY) && gameBoard[newX][newY] == 0)
						{
							coordinates initial(x, y);
							coordinates final(newX, newY);
							std::pair<coordinates, coordinates> move(initial, final);
							possibleMoves.emplace_back(move);

							newX += directionX[i];
							newY += directionY[i];
						}
					}
					break;

				// Queens
				case 7:
					// Check all eight directions
					for (int i = 0; i < 8; i++)
					{
						int directionX[8] = { 0, 1, 0, -1, 1, 1, -1, -1 };
						int directionY[8] = { 1, 0, -1, 0, 1, -1, 1, -1 };
						int newX = x + directionX[i];
						int newY = y + directionY[i];
						while (InBounds(newX, newY) && gameBoard[newX][newY] == 0)
						{
							coordinates initial(x, y);
							coordinates final(newX, newY);
							std::pair<coordinates, coordinates> move(initial, final);
							possibleMoves.emplace_back(move);

							newX += directionX[i];
							newY += directionY[i];
						}
					}
					break;

				// Kings
				case 9:
					// [Check for castle]
					// Black King
					if (color == -1 && !blackInCheck)
					{

					}

					// White King
					if (color == 1 && !whiteInCheck)
					{

					}
				case 8:
					// Check standard moves
					for (int areaX = -1; areaX <= 1; areaX++)
						for (int areaY = -1; areaY <= 1; areaY++)
						{

						}
					break;
			}
}

// Comparison Opperator
bool operator<(const GameBoard& lhs, const GameBoard& rhs)
{
	// First check if boards reference the same board
	if (&lhs == &rhs)
		return false;

	// If not, check number of pieces
	if (lhs.numBlackPieces() != rhs.numBlackPieces())
		return lhs.numBlackPieces() < rhs.numBlackPieces();

	if (lhs.numWhitePieces() != rhs.numWhitePieces())
		return lhs.numWhitePieces() < rhs.numWhitePieces();

	// If piece counts are the same, check moves since capture
	// (A board is more valuable when moves since capture is closer to 0)
	if (lhs.numMovesSinceCapture() != rhs.numMovesSinceCapture())
		return lhs.numMovesSinceCapture() > rhs.numMovesSinceCapture();

	// If the number of moves since capture are the same, compare check flags
	// (A board is more valuable when a king is not in check)
	if (lhs.isBlackInCheck() != rhs.isBlackInCheck())
		return lhs.isBlackInCheck() > rhs.isBlackInCheck();

	if (lhs.isWhiteInCheck() != rhs.isWhiteInCheck())
		return lhs.isWhiteInCheck() > rhs.isWhiteInCheck();

	
	// If the state of check is the same, calculate the fitness of the board
	// [TODO: CALCULATE FITNESS METHOD]

	// If all are equal, assume boards are equal and return false
	return false;
}