#pragma once

#include <SFML/Graphics.hpp>
#include "GameBoard.hpp"

// Struct draws sprites to the screen
struct sprites : public sf::Drawable
{
	// Explicit constructor takes a reference to a GameBoard
	sprites(GameBoard& board)
	{
		referenceBoard = &board;

		for (int i = 0; i < 6; i++)
			pieceIcons[i] = new sf::Texture;


		// Load in textures
		for (int i = 0; i < 6; i++)
			pieceIcons[i] = new sf::Texture;

		pieceIcons[0]->loadFromFile("Image Assets/Pawn.png");
		pieceIcons[1]->loadFromFile("Image Assets/Rook.png");
		pieceIcons[2]->loadFromFile("Image Assets/Knight.png");
		pieceIcons[3]->loadFromFile("Image Assets/Bishop.png");
		pieceIcons[4]->loadFromFile("Image Assets/Queen.png");
		pieceIcons[5]->loadFromFile("Image Assets/King.png");
	}

	// Destructor
	~sprites()
	{
		for (int i = 0; i < 6; i++)
			delete pieceIcons[i];
	}

	// Pointer to an existing game board
	GameBoard* referenceBoard;

	// Stores the icons for chess pieces in memory
	sf::Texture* pieceIcons[6];

	// Method to draw items to the window
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		// Texture object to load to
		sf::Texture piece;

		// Calculate the width for a tile
		int tileWidth = target.getSize().x / 8;

		// Holds new position
		int newX, newY;

		// Draw background
		for (int x = 0; x < 8; x++)
			for (int y = 0; y < 8; y++)
			{
				// Tile to draw with
				sf::RectangleShape backGround;

				// Calculate new position
				newX = x * tileWidth;
				newY = y * tileWidth;

				// Shift tile to correct position
				backGround.setPosition(sf::Vector2f((float)newX, (float)newY));

				// Strech Tile to fill board
				backGround.setSize(sf::Vector2f((float)tileWidth, (float)tileWidth));

				// Color tile
				if ((x + y) % 2 == 0)
					backGround.setFillColor(sf::Color(250, 250, 250));
				else
					backGround.setFillColor(sf::Color(5, 5, 5));

				// Draw tile
				target.draw(backGround);

				// Check if piece exists on that tile
				if ((*referenceBoard).gameBoard[x][y] != 0)
				{
					// Load the correct texture
					switch ((*referenceBoard).gameBoard[x][y])
					{
						// Black King
					case -9:
					case -8:
						backGround.setTexture(pieceIcons[5]);
						backGround.setFillColor(sf::Color(105, 105, 105));
						break;

						// Black Queen
					case -7:
						backGround.setTexture(pieceIcons[4]);
						backGround.setFillColor(sf::Color(105, 105, 105));
						break;

						// Black Bishop
					case -6:
						backGround.setTexture(pieceIcons[3]);
						backGround.setFillColor(sf::Color(105, 105, 105));
						break;

						// Black Knight
					case -5:
						backGround.setTexture(pieceIcons[2]);
						backGround.setFillColor(sf::Color(105, 105, 105));
						break;

						// Black Rook
					case -4:
					case -3:
						backGround.setTexture(pieceIcons[1]);
						backGround.setFillColor(sf::Color(105, 105, 105));
						break;

						// Black Pawn
					case -2:
					case -1:
						backGround.setTexture(pieceIcons[0]);
						backGround.setFillColor(sf::Color(105, 105, 105));
						break;

						// White Pawn
					case 1:
					case 2:
						backGround.setTexture(pieceIcons[0]);
						backGround.setFillColor(sf::Color(255, 255, 255));
						break;

						// White Rook
					case 3:
					case 4:
						backGround.setTexture(pieceIcons[1]);
						backGround.setFillColor(sf::Color(255, 255, 255));
						break;

						// White Knight
					case 5:
						backGround.setTexture(pieceIcons[2]);
						backGround.setFillColor(sf::Color(255, 255, 255));
						break;

						// White Bishop
					case 6:
						backGround.setTexture(pieceIcons[3]);
						backGround.setFillColor(sf::Color(255, 255, 255));
						break;

						// White Queen
					case 7:
						backGround.setTexture(pieceIcons[4]);
						backGround.setFillColor(sf::Color(255, 255, 255));
						break;

						// White King
					case 8:
					case 9:
						backGround.setTexture(pieceIcons[5]);
						backGround.setFillColor(sf::Color(255, 255, 255));
						break;
					}

					// Draw the piece
					target.draw(backGround);
				}
			}
	}
};