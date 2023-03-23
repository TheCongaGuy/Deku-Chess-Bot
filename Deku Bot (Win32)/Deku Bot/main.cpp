#include "Test.hpp"
#include <SFML/Graphics.hpp> // External Window Library
#include <iostream>

int main(int argc, char* argv[])
{
	// Run a test before program start
	runAllTests();

	// Chess Board
	GameBoard board;

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
		window.draw(board);
		window.display();
	}

	return 0;
}