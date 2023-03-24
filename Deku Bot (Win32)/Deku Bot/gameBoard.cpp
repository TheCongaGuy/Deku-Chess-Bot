#include "GameBoard.hpp"

// Default Constructor
GameBoard::GameBoard()
{
	// Set flags
	movesSinceCapture = 0;
	blackInCheck = whiteInCheck = false;
	whiteTurn = true;
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

		// Set the previous position to all 0 tiles
		for (int y = 0; y < 8; y++)
			previousPosition[x][y] = 0;
	}
}

// Explicit Constructor
// Takes a reference to a pre-made game board
GameBoard::GameBoard(const piece(&gameState)[8][8])
{
	// Copy the piece positions to this objects game board
	for (int x = 0; x < 8; x++)
		for (int y = 0; y < 8; y++)
		{
			gameBoard[x][y] = gameState[x][y];

			// Set the previous position to all 0 tiles
			previousPosition[x][y] = 0;
		}

	// Evaluate the game board position
	EvaluateBoard();

	// Assume a capture was just made
	movesSinceCapture = 0;

	// Assume that it is white's turn
	whiteTurn = true;
}

// Copy Constructor
GameBoard::GameBoard(const GameBoard& rhs)
{
	// Copy flags
	movesSinceCapture = rhs.movesSinceCapture;
	blackInCheck = rhs.blackInCheck;
	whiteInCheck = rhs.whiteInCheck;
	whiteTurn = rhs.whiteTurn;
	blackPieces = rhs.blackPieces;
	whitePieces = rhs.whitePieces;

	// Copy the board of the other object
	for (int x = 0; x < 8; x++)
		for (int y = 0; y < 8; y++)
		{
			gameBoard[x][y] = rhs.gameBoard[x][y];

			// Copy the previous position of the rhs
			previousPosition[x][y] = rhs.previousPosition[x][y];
		}
}

// Performs a move on the board
// Takes two coordinates; the location of the piece to be moved and a final position
// Returns true if move was made, false otherwise
bool GameBoard::MovePiece(const coordinates initial, const coordinates final)
{
	// Flag holds move validity
	bool validMove = false;

	// Calculate all legal moves for the current color's turn
	std::vector<std::pair<coordinates, coordinates>> legalMoves;

	if (whiteTurn)
		legalMoves = FindMoves(1);
	else
		legalMoves = FindMoves(-1);

	// Check to see if the given move exists in the vector of legal moves
	for (auto& move : legalMoves)
	{
		if (move.first != initial)
			continue;

		if (move.second != final)
			continue;

		validMove = true;
		break;
	}

	// If the move exists, preform the move
	if (validMove)
	{
		// Update the previous position
		for (int x = 0; x < 8; x++)
			for (int y = 0; y < 8; y++)
				previousPosition[x][y] = gameBoard[x][y];

		int oldX = initial.first;
		int oldY = initial.second;
		int newX = final.first;
		int newY = final.second;

		gameBoard[newX][newY] = gameBoard[oldX][oldY];
		gameBoard[oldX][oldY] = 0;

		// Evaluate the board
		EvaluateBoard();

		// Swap Turns
		whiteTurn = !whiteTurn;
	}

	// Return the validity of the move
	return validMove;
}

// Ranks the board for a given color
// 1 -> White | -1 -> Black
// Returns an integer representing it's fitness
int GameBoard::RankBoard(const int color) const
{
	// Fitness of the board
	int fitness = 0;

	// Flags indicate if kings exist
	bool blackKing = false, whiteKing = false;

	// First check for draws
	if (blackInCheck && whiteInCheck)
			return 0;

	// Next check for checks
	if (color == 1 && blackInCheck)
		fitness += 500;
	if (color == 1 && whiteInCheck)
		fitness -= 500;

	if (color == -1 && whiteInCheck)
		fitness += 500;
	if (color == -1 && blackInCheck)
		fitness -= 500;

	// Check each grid space
	for (int x = 0; x < 8; x++)
		for (int y = 0; y < 8; y++)
			switch (abs(gameBoard[x][y]))
			{
				// Pawns
				case 1:
				case 2:
					// For black pawns
					if (gameBoard[x][y] < 0)
					{
						// Add their progress to fitness
						if (gameBoard[x][y] * color > 0)
							fitness += (y - 1);
						else
							fitness -= (y - 1);

						// Check diagonals
						if (InBounds(x + 1, y + 1))
						{
							if (gameBoard[x][y] * color > 0)
								fitness++;
							else
								fitness--;
						}

						if (InBounds(x - 1, y + 1))
						{
							if (gameBoard[x][y] * color > 0)
								fitness++;
							else
								fitness--;
						}
					}

					// For white pawns
					if (gameBoard[x][y] > 0)
					{
						// Add their progress to fitness
						if (gameBoard[x][y] * color > 0)
							fitness += (6 - y);
						else
							fitness -= (6 - y);

						// Check diagonals
						if (InBounds(x + 1, y - 1))
						{
							if (gameBoard[x][y] * color > 0)
								fitness++;
							else
								fitness--;
						}

						if (InBounds(x - 1, y - 1))
						{
							if (gameBoard[x][y] * color > 0)
								fitness++;
							else
								fitness--;
						}
					}
					break;

				// Rooks
				case 4:
					if (gameBoard[x][y] * color > 0)
						fitness++;
					else
						fitness--;
				case 3:
					if (gameBoard[x][y] * color > 0)
						fitness += 3;
					else
						fitness -= 3;
					// Check all four directions
					for (int i = 0; i < 4; i++)
					{
						int directionX[4] = { 0, 1, 0, -1 };
						int directionY[4] = { 1, 0, -1, 0 };
						int newX = x + directionX[i];
						int newY = y + directionY[i];
						while (InBounds(newX, newY))
						{
							if (gameBoard[x][y] * color > 0)
								fitness++;
							else
								fitness--;

							if (gameBoard[newX][newY] != 0)
								break;

							newX += directionX[i];
							newY += directionY[i];
						}
					}
					break;

				// Knights
				case 5:

					if (gameBoard[x][y] * color > 0)
						fitness += 5;
					else
						fitness -= 5;

					for (int areaX = -2; areaX <= 2; areaX++)
						for (int areaY = -2; areaY <= 2; areaY++)
						{
							int distance = abs(areaX) + abs(areaY);
							if (distance == 3)
							{
								int newX = x + areaX;
								int newY = y + areaY;
								if (InBounds(newX, newY))
								{
									if (gameBoard[x][y] * color > 0)
										fitness++;
									else
										fitness--;
								}
							}
						}
					break;

				// Bishops
				case 6:
					if (gameBoard[x][y] * color > 0)
						fitness += 6;
					else
						fitness -= 6;

					// Check all four directions
					for (int i = 0; i < 4; i++)
					{
						int directionX[4] = { 1, 1, -1, -1 };
						int directionY[4] = { 1, -1, 1, -1 };
						int newX = x + directionX[i];
						int newY = y + directionY[i];
						while (InBounds(newX, newY))
						{
							if (gameBoard[x][y] * color > 0)
								fitness++;
							else
								fitness--;

							if (gameBoard[newX][newY] != 0)
								break;

							newX += directionX[i];
							newY += directionY[i];
						}
					}
					break;

				// Queens
				case 7:
					if (gameBoard[x][y] * color > 0)
						fitness += 7;
					else
						fitness -= 7;

					// Check all eight directions
					for (int i = 0; i < 8; i++)
					{
						int directionX[8] = { 0, 1, 0, -1, 1, 1, -1, -1 };
						int directionY[8] = { 1, 0, -1, 0, 1, -1, 1, -1 };
						int newX = x + directionX[i];
						int newY = y + directionY[i];
						while (InBounds(newX, newY))
						{
							if (gameBoard[x][y] * color > 0)
								fitness++;
							else
								fitness--;

							if (gameBoard[newX][newY] != 0)
								break;

							newX += directionX[i];
							newY += directionY[i];
						}
					}
					break;

				// Kings
				case 9:
				case 8:
					if (gameBoard[x][y] * color > 0)
						fitness += 20;
					else
						fitness -= 20;

					// Check standard moves
					for (int areaX = -1; areaX <= 1; areaX++)
						for (int areaY = -1; areaY <= 1; areaY++)
							if (InBounds(x + areaX, y + areaY) && (areaX != 0 || areaY != 0))
							{
								if (gameBoard[x][y] * color > 0)
									fitness++;
								else
									fitness--;
							}

					// Flag kings existance
					if (!whiteKing && gameBoard[x][y] > 0)
						whiteKing = true;
					if (!blackKing && gameBoard[x][y] < 0)
						blackKing = true;
					break;
			}


	// Check that kings exist
	if (color == 1 && !blackKing)
		return 1000;
	if (color == 1 && !whiteKing)
		return -1000;

	if (color == -1 && !whiteKing)
		return 1000;
	if (color == -1 && !blackKing)
		return -1000;

	// If kings exist, return the calculated fitness
	return fitness;
}

// Evaluates the current board and updates pieces / flags
void GameBoard::EvaluateBoard()
{
	// Assume King is not in check at start
	blackInCheck = whiteInCheck = false;

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
				gameBoard[x][4] = 1;
		}

		// Check New En Passant Opportunities
		if (gameBoard[x][4] == 1)
			if (previousPosition[x][6] == 1)
			{
				gameBoard[x][4] = 2;
				pawnMove = true;
			}

		if (gameBoard[x][3] == -1)
			if (previousPosition[x][1] == -1)
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
			if (gameBoard[x][y] == 1 && previousPosition[x][y] == 0)
				pawnMove = true;

			if (gameBoard[x][y] == -1 && previousPosition[x][y] == 0)
				pawnMove = true;

			// Check Castle Rooks
			if (x % 7 != 0 || y % 7 != 0)
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

				// Check if king castled
				if (gameBoard[2][7] == 9)
				{
					gameBoard[0][7] = 0;
					gameBoard[3][7] = 3;
				}
				if (gameBoard[6][7] == 9)
				{
					gameBoard[7][7] = 0;
					gameBoard[5][7] = 3;
				}

				// Check if king moved from start
				if (gameBoard[x][y] == 9 && (x != 4 || y != 7))
					gameBoard[x][y] = 8;
			}

			if (gameBoard[x][y] == -8 || gameBoard[x][y] == -9)
			{
				// Record king location
				blackKingX = x;
				blackKingY = y;

				// Check if king castled
				if (gameBoard[2][0] == -9)
				{
					gameBoard[0][0] = 0;
					gameBoard[3][0] = -3;
				}
				if (gameBoard[6][0] == -9)
				{
					gameBoard[7][0] = 0;
					gameBoard[5][0] = -3;
				}

				// Check if king moved from start
				if (gameBoard[x][y] == -9 && (x != 4 || y != 0))
					gameBoard[x][y] = -8;
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

	// Flags for castling
	bool castleLong = true, castleShort = true;

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
						if (InBounds(x + 1, y - 1) && (gameBoard[x + 1][y - 1] < 0 || gameBoard[x + 1][y] == -2))
						{
							coordinates initial(x, y);
							coordinates final(x + 1, y - 1);
							std::pair<coordinates, coordinates> move(initial, final);
							possibleMoves.emplace_back(move);
						}

						if (InBounds(x - 1, y - 1) && (gameBoard[x - 1][y - 1] < 0 || gameBoard[x - 1][y] == -2))
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
						while (InBounds(newX, newY) && gameBoard[newX][newY] * color <= 0)
						{
							coordinates initial(x, y);
							coordinates final(newX, newY);
							std::pair<coordinates, coordinates> move(initial, final);
							possibleMoves.emplace_back(move);

							if (gameBoard[newX][newY] * color < 0)
								break;

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
								if (InBounds(newX, newY) && gameBoard[newX][newY] * color <= 0)
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
						while (InBounds(newX, newY) && gameBoard[newX][newY] * color <= 0)
						{
							coordinates initial(x, y);
							coordinates final(newX, newY);
							std::pair<coordinates, coordinates> move(initial, final);
							possibleMoves.emplace_back(move);

							if (gameBoard[newX][newY] * color < 0)
								break;

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
						while (InBounds(newX, newY) && gameBoard[newX][newY] * color <= 0)
						{
							coordinates initial(x, y);
							coordinates final(newX, newY);
							std::pair<coordinates, coordinates> move(initial, final);
							possibleMoves.emplace_back(move);

							if (gameBoard[newX][newY] * color < 0)
								break;

							newX += directionX[i];
							newY += directionY[i];
						}
					}
					break;

				// Kings
				case 9:
					// Check castle long
					for (int i = 1; i < 4 && castleLong; i++)
					{
						// Black King
						if (color == -1 && !blackInCheck)
						{
							if (gameBoard[x - i][y] != 0 || gameBoard[0][y] != -4)
								castleLong = false;
						}

						// White King
						if (color == 1 && !whiteInCheck)
						{
							if (gameBoard[x - i][y] != 0 || gameBoard[0][y] != 4)
								castleLong = false;
						}
					}

					// Check castle short
					for (int i = 1; i < 3 && castleShort; i++)
					{
						// Black King
						if (color == -1 && !blackInCheck)
						{
							if (gameBoard[x + i][y] != 0 || gameBoard[7][y] != -4)
								castleShort = false;
						}

						// White King
						if (color == 1 && !whiteInCheck)
						{
							if (gameBoard[x + i][y] != 0 || gameBoard[7][y] != 4)
								castleShort = false;
						}
					}

					// Add the castling moves if applicable
					if (castleLong)
					{
						coordinates initial(x, y);
						coordinates final(x - 2, y);
						std::pair<coordinates, coordinates> move(initial, final);
						possibleMoves.emplace_back(move);
					}

					if (castleShort)
					{
						coordinates initial(x, y);
						coordinates final(x + 2, y);
						std::pair<coordinates, coordinates> move(initial, final);
						possibleMoves.emplace_back(move);
					}
				case 8:
					// Check standard moves
					for (int areaX = -1; areaX <= 1; areaX++)
						for (int areaY = -1; areaY <= 1; areaY++)
							if (InBounds(x + areaX, y + areaY) && gameBoard[x + areaX][y + areaY] * color <= 0 && (areaX != 0 || areaY != 0))
							{
								coordinates initial(x, y);
								coordinates final(x + areaX, y + areaY);
								std::pair<coordinates, coordinates> move(initial, final);
								possibleMoves.emplace_back(move);
							}
					break;
			}

	// Shring to save space
	possibleMoves.shrink_to_fit();
	return possibleMoves;
}