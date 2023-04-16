#pragma once

#include <fstream>
#include <string>

#include "GameBoard.hpp"
#include "DekuBot.hpp"
#include "Sprite.h"

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

// Runs a game versus two AI's to allow for observation of moves
void AIGame();

// Tests the AI with certain board configurations
void AISituations();

// Times Each Board Method and writes the results to a log
// Additionally logs info on what resources are being used
void timeTest();