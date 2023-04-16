#include "GameBoard.hpp"

// Default Constructor
GameBoard::GameBoard() : movesSinceCapture(0), blackInCheck(false), whiteInCheck(false), draw(false), whiteTurn(true),
						 blackPieces(16), whitePieces(16)
{
	// Configuration of back ranks
	const piece backRanks[8] = { 4, 5, 6, 7, 9, 6, 5, 4 };

	for (int x = 0; x < 8; x++)
	{
		// Set front ranks for each color
		gameBoard[x][1] = -1;
		gameBoard[x][6] = 1;

		// Set back ranks for each color
		gameBoard[x][0] = -backRanks[x];
		gameBoard[x][7] = backRanks[x];

		// Set the previous position to all 0 tiles
		std::memset(previousPosition, 0, sizeof(previousPosition));
	}
}

// Explicit Constructor
// Takes a reference to a pre-made game board
GameBoard::GameBoard(const piece(&gameState)[8][8]) : movesSinceCapture(0), whiteTurn(true), draw(false)
{
	// Copy the piece positions to this objects game board
	std::copy(&gameState[0][0], &gameState[0][0] + 64, &gameBoard[0][0]);

	// Set the previous position to all 0 tiles
	std::memset(previousPosition, 0, sizeof(previousPosition));

	EvaluateBoard();
}

// Copy Constructor
GameBoard::GameBoard(const GameBoard& rhs) : movesSinceCapture(rhs.movesSinceCapture), blackInCheck(rhs.blackInCheck),
											 whiteInCheck(rhs.whiteInCheck), draw(rhs.draw), whiteTurn(rhs.whiteTurn),
											 blackPieces(rhs.blackPieces), whitePieces(rhs.whitePieces)
{
	// Copy the board of the other object
	std::memcpy(gameBoard, rhs.gameBoard, sizeof(gameBoard));

	// Copy the previous position of the other object
	std::memcpy(previousPosition, rhs.previousPosition, sizeof(previousPosition));
}

// Performs a move on the board
// Takes two coordinates; the location of the piece to be moved and a final position
// Returns true if move was made, false otherwise
bool GameBoard::MovePiece(const coordinates initial, const coordinates final)
{
	// Calculate all legal moves for the current color's turn
	std::vector<std::pair<coordinates, coordinates>> legalMoves = whiteTurn ? FindMoves(1) : FindMoves(-1);

	// Check to see if the given move exists in the vector of legal moves
	auto move = std::find_if (legalMoves.begin(), legalMoves.end(), [&](const auto& move) 
		{ return move.first == initial && move.second == final; }
	);

	// If the move exists, perform the move
	if (move != legalMoves.end())
	{
		// Update the previous position
		std::memcpy(previousPosition, gameBoard, sizeof(gameBoard));

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

		return true;
	}

	return false;
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
	if (draw)
			return 0;

	// Check each grid space
	for (int x = 0; x < 8; x++)
		for (int y = 0; y < 8; y++)
		{
			// Save the value of the piece
			int pieceValue = gameBoard[x][y] * color;

			// Add a bonus if a piece was captured
			if (pieceValue > 0 && previousPosition[x][y] * color < 0)
				fitness += abs(previousPosition[x][y]);
			if (pieceValue < 0 && previousPosition[x][y] * color > 0)
				fitness -= abs(previousPosition[x][y]);

			switch (abs(pieceValue))
			{
				// Pawns
				case 1:
				case 2:
				{
					// Record the direction of movement for the pawn
					const int pawnDir = (gameBoard[x][y] > 0) ? -1 : 1;

					// Add the pawn's value to fitness
					fitness += ((pieceValue > 0) ? 2 : -2);

					// Check diagonals
					for (int dx : {-1, 1})
						if (InBounds(x + dx, y + pawnDir))
						{
							if (pieceValue > 0)
								fitness++;
							else
								fitness--;
						}
					break;
				}

				// Rooks
				case 4:
				case 3:
				{
					const int increment = (pieceValue > 0) ? 1 : -1;
					fitness += increment * 14;

					for (int i = 0; i < 4; i++)
					{
						const int directionX[4] = { 0, 1, 0, -1 };
						const int directionY[4] = { 1, 0, -1, 0 };
						int newX = x + directionX[i];
						int newY = y + directionY[i];
						while (InBounds(newX, newY))
						{
							fitness += increment;

							if (gameBoard[newX][newY] != 0)
								break;

							newX += directionX[i];
							newY += directionY[i];
						}
					}
					break;
				}

				// Knights
				case 5:
				{
					const int increment = (pieceValue > 0) ? 1 : -1;
					fitness += increment * 8;

					for (int areaX : {-2, -1, 1, 2})
						for (int areaY : {-2, -1, 1, 2})
						{
							if (abs(areaX) != abs(areaY))
							{
								const int newX = x + areaX;
								const int newY = y + areaY;
								if (InBounds(newX, newY))
								{
									fitness += increment;
								}
							}
						}
					break;
				}

				// Bishops
				case 6:
				{
					const int increment = (pieceValue > 0) ? 1 : -1;
					fitness += increment * 13;

					const int directionX[4] = { 1, 1, -1, -1 };
					const int directionY[4] = { 1, -1, 1, -1 };
					for (int i = 0; i < 4; i++)
					{
						int newX = x + directionX[i];
						int newY = y + directionY[i];
						while (InBounds(newX, newY))
						{
							fitness += increment;

							if (gameBoard[newX][newY] != 0)
								break;

							newX += directionX[i];
							newY += directionY[i];
						}
					}
					break;
				}

				// Queens
				case 7:
				{
					const int increment = (pieceValue > 0) ? 1 : -1;
					fitness += increment * 27;

					const int directionX[8] = { 0, 1, 0, -1, 1, 1, -1, -1 };
					const int directionY[8] = { 1, 0, -1, 0, 1, -1, 1, -1 };
					// Check all eight directions
					for (int i = 0; i < 8; i++)
					{
						int newX = x + directionX[i];
						int newY = y + directionY[i];
						while (InBounds(newX, newY))
						{
							fitness += increment;

							if (gameBoard[newX][newY] != 0)
								break;

							newX += directionX[i];
							newY += directionY[i];
						}
					}
					break;
				}

				// Kings
				case 9:
				case 8:
					// Flag kings existance
					if (!whiteKing && gameBoard[x][y] > 0)
						whiteKing = true;
					if (!blackKing && gameBoard[x][y] < 0)
						blackKing = true;
					break;
			}
		}


	if (color == 1) 
	{
		if (!blackKing)
			return 1000;
		if (!whiteKing)
			return -1000;
	}
	else 
	{
		if (!whiteKing)
			return 1000;
		if (!blackKing)
			return -1000;
	}

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
		// Quick reference variables
		piece& pawnSecondRow = gameBoard[x][2];
		piece& pawnThirdRow = gameBoard[x][3];
		piece& pawnFourthRow = gameBoard[x][4];
		piece& pawnFifthRow = gameBoard[x][5];

		piece& pawnTopRow = gameBoard[x][0];
		piece& pawnBottomRow = gameBoard[x][7];

		// Check Past En Passant Opportunities
		if (pawnThirdRow == -2)
		{
			if (pawnSecondRow == 1)
				pawnThirdRow = 0;
			else
				pawnThirdRow = -1;
		}
		else if (pawnFourthRow == 2)
		{
			if (pawnFifthRow == -1)
				pawnFourthRow = 0;
			else
				pawnFourthRow = 1;
		}

		// Check New En Passant Opportunities
		if (pawnFourthRow == 1 && previousPosition[x][6] == 1)
			{
				pawnFourthRow = 2;
				pawnMove = true;
			}
		else if (pawnThirdRow == -1 && previousPosition[x][1] == -1)
			{
				pawnThirdRow = -2;
				pawnMove = true;
			}

		// Check Promotions
		if (pawnBottomRow == -1)
		{
			pawnBottomRow = -7;
			pawnMove = true;
		}
		else if (pawnTopRow == 1)
		{
			pawnTopRow = 7;
			pawnMove = true;
		}

		// Check the rest of the gameboard
		for (int y = 0; y < 8; y++)
		{
			piece& currentPiece = gameBoard[x][y];
			piece& previousPiece = previousPosition[x][y];
			// Count black pieces
			if (currentPiece < 0)
				currentBlackPieces++;

			// Count white pieces
			else if (currentPiece > 0)
				currentWhitePieces++;

			// Check pawn movement
			if (currentPiece == 1 && previousPiece == 0)
				pawnMove = true;

			else if (currentPiece == -1 && previousPiece == 0)
				pawnMove = true;

			// Check Castle Rooks
			if (x % 7 != 0 || y % 7 != 0)
			{
				if (currentPiece == 4)
					currentPiece = 3;

				else if (currentPiece == -4)
					currentPiece = -3;
			}

			// Check Kings
			if (currentPiece == 8 || currentPiece == 9)
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
				else if (gameBoard[6][7] == 9)
				{
					gameBoard[7][7] = 0;
					gameBoard[5][7] = 3;
				}

				// Check if king moved from start
				if (currentPiece == 9 && (x != 4 || y != 7))
					currentPiece = 8;
			}

			else if (currentPiece == -8 || currentPiece == -9)
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
				else if (gameBoard[6][0] == -9)
				{
					gameBoard[7][0] = 0;
					gameBoard[5][0] = -3;
				}

				// Check if king moved from start
				if (currentPiece == -9 && (x != 4 || y != 0))
					currentPiece = -8;
			}
		}
	}

	// Update flags
	if (!pawnMove && blackPieces == currentBlackPieces && whitePieces == currentWhitePieces)
		movesSinceCapture++;
	else
		movesSinceCapture = 0;

	blackPieces = currentBlackPieces;
	whitePieces = currentWhitePieces;

	// Check for draws
	if (movesSinceCapture == 100)
	{
		draw = true;
		return;
	}

	// Check for king existance
	if (blackKingX == -1 || whiteKingX == -1)
		return;

	// Check for pawn checks
	for (int offset : {1, -1})
	{
		piece& targetSquareBlack = gameBoard[blackKingX + offset][blackKingY + 1];
		piece& targetSquareWhite = gameBoard[whiteKingX + offset][whiteKingY - 1];

		if (targetSquareBlack == 1 || targetSquareBlack == 2)
			blackInCheck = true;

		else if (targetSquareWhite == -1 || targetSquareWhite == -2)
			whiteInCheck = true;
	}

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
		int newY = blackKingY + directionY[i];
		while (InBounds(newX, newY))
		{
			piece& targetSquare = gameBoard[newX][newY];
			// Friendly piece blocks path
			if (targetSquare < 0)
				break;

			// Bishop or Queen creates check
			if (targetSquare == 6 || targetSquare == 7)
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
		int newY = whiteKingY + directionY[i];
		while (InBounds(newX, newY))
		{
			piece& targetSquare = gameBoard[newX][newY];

			// Friendly piece blocks path
			if (targetSquare > 0)
				break;

			// Bishop or Queen creates check
			if (targetSquare == -6 || targetSquare == -7)
				whiteInCheck = true;

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
			piece& targetSquare = gameBoard[newX][newY];
			// Friendly piece blocks path
			if (targetSquare < 0)
				break;

			// Rook or Queen creates check
			if (targetSquare == 3 || targetSquare == 4 || targetSquare == 7)
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
			piece& targetSquare = gameBoard[newX][newY];
			// Friendly piece blocks path
			if (targetSquare > 0)
				break;

			// Rook or Queen creates check
			if (targetSquare == -3 || targetSquare == -4 || targetSquare == -7)
				whiteInCheck = true;

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
	possibleMoves.reserve(137);

	// Flags for castling
	bool castleLong = true, castleShort = true;

	// Check each grid space
	for (int x = 0; x < 8; x++)
		for (int y = 0; y < 8; y++)
			switch (gameBoard[x][y] * color)
			{
				// Pawns
				case 1:
					switch (color) 
					{
						case -1: // For black pawns
						// Check for En Passant
						if (y == 1 && gameBoard[x][2] == 0 && gameBoard[x][3] == 0)
							possibleMoves.emplace_back(std::piecewise_construct, std::forward_as_tuple(x, y), std::forward_as_tuple(x, 3));
						break;

						case 1: // For white pawns
							// Check for En Passant
							if (y == 6 && gameBoard[x][5] == 0 && gameBoard[x][4] == 0)
								possibleMoves.emplace_back(std::piecewise_construct, std::forward_as_tuple(x, y), std::forward_as_tuple(x, 4));
							break;
					}
				case 2:
					switch (color)
					{
						// black pawns
						case -1:
							// Check space infront
							if (InBounds(x, y + 1) && gameBoard[x][y + 1] == 0)
								possibleMoves.emplace_back(std::piecewise_construct, std::forward_as_tuple(x, y), std::forward_as_tuple(x, y + 1));

							// Check diagonals
							if (InBounds(x + 1, y + 1) && (gameBoard[x + 1][y + 1] > 0 || gameBoard[x + 1][y] == 2))
								possibleMoves.emplace_back(std::piecewise_construct, std::forward_as_tuple(x, y), std::forward_as_tuple(x + 1, y + 1));

							if (InBounds(x - 1, y + 1) && (gameBoard[x - 1][y + 1] > 0 || gameBoard[x - 1][y] == 2))
								possibleMoves.emplace_back(std::piecewise_construct, std::forward_as_tuple(x, y), std::forward_as_tuple(x - 1, y + 1));
						break;

						// white pawns
						case 1:
							// Check space infront
							if (InBounds(x, y - 1) && gameBoard[x][y - 1] == 0)
								possibleMoves.emplace_back(std::piecewise_construct, std::forward_as_tuple(x, y), std::forward_as_tuple(x, y - 1));

							// Check diagonals
							if (InBounds(x + 1, y - 1) && (gameBoard[x + 1][y - 1] < 0 || gameBoard[x + 1][y] == -2))
								possibleMoves.emplace_back(std::piecewise_construct, std::forward_as_tuple(x, y), std::forward_as_tuple(x + 1, y - 1));

							if (InBounds(x - 1, y - 1) && (gameBoard[x - 1][y - 1] < 0 || gameBoard[x - 1][y] == -2))
								possibleMoves.emplace_back(std::piecewise_construct, std::forward_as_tuple(x, y), std::forward_as_tuple(x - 1, y - 1));
						break;
					}
					break;

				// Rooks
				case 3:
				case 4:
				{
					// Check all four directions
					int directionX[4] = { 0, 1, 0, -1 };
					int directionY[4] = { 1, 0, -1, 0 };
					for (int i = 0; i < 4; i++)
					{
						int newX = x + directionX[i];
						int newY = y + directionY[i];
						while (InBounds(newX, newY) && gameBoard[newX][newY] * color <= 0)
						{
							possibleMoves.emplace_back(std::piecewise_construct, std::forward_as_tuple(x, y), std::forward_as_tuple(newX, newY));

							if (gameBoard[newX][newY] * color < 0)
								break;

							newX += directionX[i];
							newY += directionY[i];
						}
					}
					break;
				}

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
									possibleMoves.emplace_back(std::piecewise_construct, std::forward_as_tuple(x, y), std::forward_as_tuple(newX, newY));
							}
						}
					break;

				// Bishops
				case 6:
				{
					int directionX[4] = { 1, 1, -1, -1 };
					int directionY[4] = { 1, -1, 1, -1 };
					// Check all four directions
					for (int i = 0; i < 4; i++)
					{
						int newX = x + directionX[i];
						int newY = y + directionY[i];
						while (InBounds(newX, newY) && gameBoard[newX][newY] * color <= 0)
						{
							possibleMoves.emplace_back(std::piecewise_construct, std::forward_as_tuple(x, y), std::forward_as_tuple(newX, newY));

							if (gameBoard[newX][newY] * color < 0)
								break;

							newX += directionX[i];
							newY += directionY[i];
						}
					}
					break;
				}

				// Queens
				case 7:
				{
					int directionX[8] = { 0, 1, 0, -1, 1, 1, -1, -1 };
					int directionY[8] = { 1, 0, -1, 0, 1, -1, 1, -1 };
					// Check all eight directions
					for (int i = 0; i < 8; i++)
					{
						int newX = x + directionX[i];
						int newY = y + directionY[i];
						while (InBounds(newX, newY) && gameBoard[newX][newY] * color <= 0)
						{
							possibleMoves.emplace_back(std::piecewise_construct, std::forward_as_tuple(x, y), std::forward_as_tuple(newX, newY));

							if (gameBoard[newX][newY] * color < 0)
								break;

							newX += directionX[i];
							newY += directionY[i];
						}
					}
					break;
				}

				// Kings
				case 9:
					// Check castle long
					for (int i = 1; i < 4 && castleLong; i++)
					{
						// Black King
						if (color == -1)
							if (gameBoard[x - i][y] != 0 || gameBoard[0][y] != -4 || blackInCheck)
								castleLong = false;

						// White King
						if (color == 1)
							if (gameBoard[x - i][y] != 0 || gameBoard[0][y] != 4 || whiteInCheck)
								castleLong = false;
					}

					// Check castle short
					for (int i = 1; i < 3 && castleShort; i++)
					{
						// Black King
						if (color == -1)
							if (gameBoard[x + i][y] != 0 || gameBoard[7][y] != -4 || blackInCheck)
								castleShort = false;

						// White King
						if (color == 1)
							if (gameBoard[x + i][y] != 0 || gameBoard[7][y] != 4 || whiteInCheck)
								castleShort = false;
					}

					// Add the castling moves if applicable
					if (castleLong)
						possibleMoves.emplace_back(std::piecewise_construct,std::forward_as_tuple(x, y), std::forward_as_tuple(x - 2, y));

					if (castleShort)
						possibleMoves.emplace_back(std::piecewise_construct, std::forward_as_tuple(x, y), std::forward_as_tuple(x + 2, y));
				case 8:
					// Check standard moves
					for (int areaX = -1; areaX <= 1; areaX++)
						for (int areaY = -1; areaY <= 1; areaY++)
							if (InBounds(x + areaX, y + areaY) && gameBoard[x + areaX][y + areaY] * color <= 0 && (areaX != 0 || areaY != 0))
							{
								possibleMoves.emplace_back(std::piecewise_construct, std::forward_as_tuple(x, y), std::forward_as_tuple(x + areaX, y + areaY));
							}
					break;
			}

	// Shring to save space
	possibleMoves.shrink_to_fit();
	return possibleMoves;
}