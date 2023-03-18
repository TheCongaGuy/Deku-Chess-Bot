#include "Test.hpp"

// Run all tests
void runAllTests()
{
	testBoardConstructor();
}

// Stress Tests For Game Board Functionality
void testBoardConstructor()
{
	// Test Board
	GameBoard Test;

	// Default configuration of back ranks
	piece backRanks[8] = {4, 5, 6, 7, 9, 6, 5, 4};

	for (int x = 0; x < 8; x++)
	{
		// [Check that color values are correct]
		// Fail if any piece in the first two rows aren't black
		for (int y = 0; y < 2; y++)
			if (Test.gameBoard[x][y] > 0)
			{
				std::cout << "Failed Black Side Start" << std::endl;
				exit(-1);
			}

		// Fail if any piece in the last two rows aren't white
		for (int y = 6; y < 8; y++)
			if (Test.gameBoard[x][y] < 0)
			{
				std::cout << "Failed White Side Start" << std::endl;
				exit(-1);
			}

		// Fail if any piece in the front ranks are not pawns
		if (Test.gameBoard[x][1] != -1 || Test.gameBoard[x][6] != 1)
		{
			std::cout << "Failed Front Rank Start" << std::endl;
			exit(-1);
		}

		// Fail if any piece in the back ranks do not allign with starting configuration
		if (Test.gameBoard[x][0] != -backRanks[x] || Test.gameBoard[x][7] != backRanks[x])
		{
			std::cout << "Failed Back Rank Start" << std::endl;
			exit(-1);
		}
	}
}