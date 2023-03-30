#include "Test.hpp"
#include "DekuBot.hpp"
#include "Sprite.h"
#include <SFML/Graphics.hpp> // External Window Library
#include <iostream>

int main(int argc, char* argv[])
{
	int aiColor = 0;

	// Get Piece Information
	std::cout << "What Color Is The AI? (w/b): ";
	std::string input;
	std::cin >> input;
	if (input[0] == 'b')
		aiColor = -1;
	else
		aiColor = 1;

	// Display Choice
	std::cout << "AI Color: ";
	if (aiColor == -1)
		std::cout << "Black" << std::endl;
	else
		std::cout << "White" << std::endl;

	// Get Max Search Time from user
	std::cout << "How long may the AI search in minutes? (Negative numbers allow for no limit): ";
	std::cin >> input;

	float maxTime = std::stof(input);

	if (maxTime < 0)
	{
		std::cout << "WARNING: You have selected no limit to the search time. This may have unknown concequences " << std::endl
				  << "regarding the application, results, or total time spent computing. If you would like to cap" << std::endl
				  << "the search, please enter a positive integer now." << std::endl;

		std::cin >> input;
		maxTime = std::stoi(input);

		if (maxTime < 0)
			maxTime = INT32_MAX / 60000;
	}

	// Display Max Search Time
	std::cout << "Deku Target Search Time: " << maxTime << " Minute(s)" << std::endl;

	std::cout << "Setting Up. . ." << std::endl;

	// Run a test before program start
	runAllTests();

	std::cout << "Pre-Tests Passed, Instantiating AI.  .  ." << std::endl;

	// Chess Board
	GameBoard board;

	// AI
	DekuBot deku(&board, aiColor);

	// Sprite Renderer
	sprites drawable(board);

	// Coordinates of the mouse
	coordinates onClick(-1, -1);
	coordinates onRelease(-1, -1);

	// Get the resolution of the user's monitor
	sf::VideoMode userMonitor = sf::VideoMode::getDesktopMode();

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

	// Main Loop
	while (window.isOpen())
	{
		// Event Manager
		sf::Event event;
		if (window.pollEvent(event))
		{
			// Close window when red x is pressed
			if (event.type == sf::Event::Closed)
				window.close();

			// Log initial coordinates of mouse press
			if (event.type == sf::Event::MouseButtonPressed)
			{
				// Tile Width
				int tileWidth = window.getSize().x / 8;

				onClick.first = sf::Mouse::getPosition(window).x / tileWidth;
				onClick.second = sf::Mouse::getPosition(window).y / tileWidth;
			}

			// Register input on mouse release
			if (event.type == sf::Event::MouseButtonReleased)
			{
				int tileWidth = window.getSize().x / 8;

				onRelease.first = sf::Mouse::getPosition(window).x / tileWidth;
				onRelease.second = sf::Mouse::getPosition(window).y / tileWidth;

				board.MovePiece(onClick, onRelease);
			}
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