#include "Test.hpp"
#include <iostream>

// Run all tests
void runAllTests()
{
	testBoardConstructor();
	testComparison();
}

// Test Game Board Constructors
void testBoardConstructor()
{
	// Initial setup position
	piece initialStart[8][8] = {
		{-4, -1, 0, 0, 0, 0, 1, 4 },
		{-5, -1, 0, 0, 0, 0, 1, 5 },
		{-6, -1, 0, 0, 0, 0, 1, 6 },
		{-7, -1, 0, 0, 0, 0, 1, 7 },
		{-9, -1, 0, 0, 0, 0, 1, 9 },
		{-6, -1, 0, 0, 0, 0, 1, 6 },
		{-5, -1, 0, 0, 0, 0, 1, 5 },
		{-4, -1, 0, 0, 0, 0, 1, 4 },
	};

	// Test Boards
	GameBoard defaultTest;
	GameBoard explicitTest(initialStart);
	GameBoard copyTest(explicitTest);

	// Default configuration of back ranks
	piece backRanks[8] = { 4, 5, 6, 7, 9, 6, 5, 4 };

	for (int x = 0; x < 8; x++)
	{
		// [Check that color values are correct]
		// Fail if any piece in the first two rows aren't black
		for (int y = 0; y < 2; y++)
		{
			if (defaultTest.gameBoard[x][y] >= 0)
			{
				std::cout << "Failed Black Side Start" << std::endl;
				exit(-1);
			}
			if (explicitTest.gameBoard[x][y] >= 0)
			{
				std::cout << "Failed Black Side Start" << std::endl;
				exit(-2);
			}
			if (copyTest.gameBoard[x][y] >= 0)
			{
				std::cout << "Failed Black Side Start" << std::endl;
				exit(-3);
			}
		}

		// Fail if any piece in the last two rows aren't white
		for (int y = 6; y < 8; y++)
		{
			if (defaultTest.gameBoard[x][y] <= 0)
			{
				std::cout << "Failed White Side Start" << std::endl;
				exit(-1);
			}
			if (explicitTest.gameBoard[x][y] <= 0)
			{
				std::cout << "Failed White Side Start" << std::endl;
				exit(-2);
			}
			if (copyTest.gameBoard[x][y] <= 0)
			{
				std::cout << "Failed White Side Start" << std::endl;
				exit(-3);
			}
		}

		// Fail if any piece in the front ranks are not pawns
		if (defaultTest.gameBoard[x][1] != -1 || defaultTest.gameBoard[x][6] != 1)
		{
			std::cout << "Failed Front Rank Start" << std::endl;
			exit(-1);
		}
		if (explicitTest.gameBoard[x][1] != -1 || explicitTest.gameBoard[x][6] != 1)
		{
			std::cout << "Failed Front Rank Start" << std::endl;
			exit(-2);
		}
		if (copyTest.gameBoard[x][1] != -1 || copyTest.gameBoard[x][6] != 1)
		{
			std::cout << "Failed Front Rank Start" << std::endl;
			exit(-3);
		}

		// Fail if any piece in the back ranks do not allign with starting configuration
		if (defaultTest.gameBoard[x][0] != -backRanks[x] || defaultTest.gameBoard[x][7] != backRanks[x])
		{
			std::cout << "Failed Back Rank Start" << std::endl;
			exit(-1);
		}
		if (explicitTest.gameBoard[x][0] != -backRanks[x] || explicitTest.gameBoard[x][7] != backRanks[x])
		{
			std::cout << "Failed Back Rank Start" << std::endl;
			exit(-2);
		}
		if (copyTest.gameBoard[x][0] != -backRanks[x] || copyTest.gameBoard[x][7] != backRanks[x])
		{
			std::cout << "Failed Back Rank Start" << std::endl;
			exit(-3);
		}
	}

	// Fail if flags are not correct
	if (defaultTest.isBlackInCheck())
	{
		std::cout << "Failed Black Check Flag" << std::endl;
		exit(-1);
	}
	if (explicitTest.isBlackInCheck())
	{
		std::cout << "Failed Black Check Flag" << std::endl;
		exit(-2);
	}
	if (copyTest.isBlackInCheck())
	{
		std::cout << "Failed Black Check Flag" << std::endl;
		exit(-3);
	}

	if (defaultTest.isWhiteInCheck())
	{
		std::cout << "Failed White Check Flag" << std::endl;
		exit(-1);
	}
	if (explicitTest.isWhiteInCheck())
	{
		std::cout << "Failed White Check Flag" << std::endl;
		exit(-2);
	}
	if (copyTest.isWhiteInCheck())
	{
		std::cout << "Failed White Check Flag" << std::endl;
		exit(-3);
	}

	if (defaultTest.numMovesSinceCapture() != 0)
	{
		std::cout << "Failed Moves Since Capture Flag" << std::endl;
		exit(-1);
	}
	if (explicitTest.numMovesSinceCapture() != 0)
	{
		std::cout << "Failed Moves Since Capture Flag" << std::endl;
		exit(-2);
	}
	if (copyTest.numMovesSinceCapture() != 0)
	{
		std::cout << "Failed Moves Since Capture Flag" << std::endl;
		exit(-3);
	}

	if (defaultTest.numBlackPieces() != 16)
	{
		std::cout << "Failed Black Pieces Flag" << std::endl;
		exit(-1);
	}
	if (explicitTest.numBlackPieces() != 16)
	{
		std::cout << "Failed Black Pieces Flag" << std::endl;
		exit(-2);
	}
	if (copyTest.numBlackPieces() != 16)
	{
		std::cout << "Failed Black Pieces Flag" << std::endl;
		exit(-3);
	}

	if (defaultTest.numWhitePieces() != 16)
	{
		std::cout << "Failed White Pieces Flag" << std::endl;
		exit(-1);
	}
	if (explicitTest.numWhitePieces() != 16)
	{
		std::cout << "Failed White Pieces Flag" << std::endl;
		exit(-2);
	}
	if (copyTest.numWhitePieces() != 16)
	{
		std::cout << "Failed White Pieces Flag" << std::endl;
		exit(-3);
	}
}

// Test Comparison Operator
void testComparison()
{
	// Lesser board configuration
	piece lesserBoard[8][8] = {
		{-4, -1, 0, 0, 0, 0, 1, 4 },
		{-5, -1, 0, 0, 0, 0, 1, 5 },
		{-6, -1, 0, 0, 0, 0, 1, 6 },
		{-7, -1, 0, 0, 0, 0, 1, 7 },
		{-9, -1, 0, 0, 0, 0, 0, 9 },
		{-6, -1, 0, 0, 0, 0, 1, 6 },
		{-5, -1, 0, 0, 0, 0, 1, 5 },
		{-4, -1, 0, 0, 0, 0, 1, 4 },
	};

	// Test Boards
	GameBoard defaultBoard;
	GameBoard firstLesser(lesserBoard);

	// Check if quick equal test returns false
	if (defaultBoard < defaultBoard)
	{
		std::cout << "Failed Self Comparison Test" << std::endl;
		exit(-1);
	}

	// Check if first comparison step is correct
	if (defaultBoard < lesserBoard)
	{
		std::cout << "Failed First Comparison Step" << std::endl;
		exit(-1);
	}
	if (!(lesserBoard < defaultBoard))
	{
		std::cout << "Failed First Comparison Step" << std::endl;
		exit(-2);
	}
}