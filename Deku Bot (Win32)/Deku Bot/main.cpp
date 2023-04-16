#include <SFML/Graphics.hpp> // External Window Library
#include <iostream>
#include <omp.h>

#include "DekuBot.hpp"
#include "Test.hpp"
#include "Helper.h"
#include "Sprite.h"

int main(int argc, char* argv[])
{
	/* ~~~~~~~~~~ DEV CONSTANTS ~~~~~~~~~~ */
	// Number of Cores Reserved for Other Tasks | Cores not used by program
	const int ReservedCores = omp_get_num_procs() / 4; // save 25% of user's cores


	/* ----- Global Variables ----- */
	// Chess Board
	GameBoard board;

	// User's monitor resolution
	sf::VideoMode userMonitor;

	// Sprite Renderer
	sprites drawable(board);

	// Mouse Input Coordinates
	coordinates onClick(-1, -1), onRelease(-1, -1);

	// AI Color
	const int aiColor = getUserColor();

	// AI
	DekuBot deku(board, aiColor);

	// Get Max Search Time from user
	const float maxTime = getSearchTime();

	// Number of Available Cores
	const int numCores = std::max(omp_get_num_procs() - ReservedCores, 1);	// leave 2 cores for other processes.
																			// default to 1 if machine has less than
																			// two cores.


	// Display Choice
	std::cout << "AI Color: ";
	switch (aiColor) {
	case -1:
		std::cout << "Black" << std::endl;
		break;

	case 1:
		std::cout << "White" << std::endl;
		break;
	}

	// Display Max Search Time
	std::cout << "Deku Target Search Time: " << maxTime << " Minute(s)" << std::endl;

	/* 
		----- Information Collected, Move On To Setup ----- 
	*/

	std::cout << "Setting Up. . ." << std::endl;

	// Set Up Thread Count
	omp_set_num_threads(numCores);

	// Run a test before program start
	runAllTests();
	std::cout << "Pre-Tests Passed, Instantiating AI.  .  ." << std::endl;

	// Get the resolution of the user's monitor
	try {
		userMonitor = sf::VideoMode::getDesktopMode();
	}
	catch (std::exception& e) {
		std::cerr << "Failed to get desktop mode: " << e.what() << std::endl;
		std::cerr << "Used to adjust size of chess board. Check Graphics Drivers / Display Settings" << std::endl;
		return -10; // exit the program with an error code
	}

	// Find the smallest dimension
	unsigned int smallestDimension = userMonitor.height;

	if (smallestDimension > userMonitor.width)
		smallestDimension = userMonitor.width;

	// Scale the window to 3/4 the user's minimum screen dimension
	smallestDimension = (unsigned int)(smallestDimension * (3.f / 4.f));

	// Generate a new VideoMode for the window
	sf::VideoMode gameWindow(smallestDimension, smallestDimension);

	std::cout << "Setup Complete" << std::endl;

	// Render Window
	sf::RenderWindow window(gameWindow, "The Great Deku Bot");

	// Width and Height of a tile
	const int tileSize = window.getSize().x / 8;

	// Main Loop
	while (window.isOpen())
	{
		/* --- Loop Variables --- */
		// Event Queue
		sf::Event event;

		// Event Manager
		if (window.pollEvent(event))
			switch (event.type)
			{
				case sf::Event::Closed:
					window.close();
					break;
				case sf::Event::MouseButtonPressed:
					onClick.first = sf::Mouse::getPosition(window).x / tileSize;
					onClick.second = sf::Mouse::getPosition(window).y / tileSize;
					break;
				case sf::Event::MouseButtonReleased:
					onRelease.first = sf::Mouse::getPosition(window).x / tileSize;
					onRelease.second = sf::Mouse::getPosition(window).y / tileSize;

					board.MovePiece(onClick, onRelease);
					break;
			}

		// Window Refresh
		window.clear();
		window.draw(drawable);
		window.display();

		// Make an AI Move if it is AI's turn
		if (aiColor == 1 && board.whosTurn() || aiColor == -1 && !board.whosTurn())
			deku.MakeMove(maxTime);
	}

	return 0;
}