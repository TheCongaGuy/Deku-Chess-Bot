#include "Test.hpp"
#include <SFML/Graphics.hpp> // External Window Library

int main(int argc, char* argv[])
{
	// Run a test before program start
	runAllTests();

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
			if (event.type == sf::Event::Closed)
				window.close();

		// Window Refresh
		window.clear();
		window.display();
	}

	return 0;
}