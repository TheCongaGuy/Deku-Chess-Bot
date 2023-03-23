#pragma once

#include "GameBoard.hpp"

// White Box Testing Struct
struct BoardTest : public GameBoard
{
	// Gets the number of calculated moves for a given color (1 -> White | -1 -> Black)
	int NumberOfMoves(const int color)
	{
		return FindMoves(color).size();
	}
};

// Run all tests
void runAllTests();

// Test Game Board Constructors
void testBoardConstructor();

// Test Game Board Fitness Method
void testBoardFitness();

// Test Movement Method
void testMoveMethod();