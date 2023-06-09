#pragma once

#include <SFML/Graphics.hpp> // External Graphics Library
#include <map>
#include <vector>

// Define a piece as an 8 bit integer
typedef __int8 piece;

// Define a coordinate as a pair of positive integers
typedef std::pair<unsigned int, unsigned int> coordinates;

// Holds information about the current game state
class GameBoard
{
public:
	// Default Constructor
	GameBoard();

	// Explicit Constructor
	// Takes a reference to a pre-made game board
	GameBoard(const piece(&gameState)[8][8]);

	// Copy Constructor
	GameBoard(const GameBoard& rhs);

	// Performs a move to the board
	// Takes two coordinates; the location of the piece to be moved and a final position
	// Returns true if move was made, false otherwise
	bool MovePiece(const coordinates initial, const coordinates final);

	// Ranks the board for a given color
	// Takes the color to favor: 1 -> White | -1 -> Black
	// Returns an integer representing that color's fitness
	int RankBoard(const int color) const;

	// Finds all possible moves for a given color (1 for white, -1 for black)
	// Returns a vector of moves (pairs of coordinates)
	// pair.first -> initial position | pair.second -> final position
	std::vector<std::pair<coordinates, coordinates>> FindMoves(int color) const;

	// Returns true if black is in check
	bool isBlackInCheck() const
	{ return blackInCheck; }

	// Returns true if white is in check
	bool isWhiteInCheck() const
	{ return whiteInCheck; }

	// Returns true if it is white's turn
	bool whosTurn() const
	{ return whiteTurn; }

	// Returns the number of moves since a capture or pawn move
	int numMovesSinceCapture() const
	{ return movesSinceCapture; }

	// Returns number of black pieces
	int numBlackPieces() const
	{ return blackPieces; }

	// Returns number of white pieces
	int numWhitePieces() const
	{ return whitePieces; }

	// ----- Data Members ----- \\

	// A 2D Array of pieces representing a game board; Top left is (0, 0)
	piece gameBoard[8][8] = { 0 };

protected:
	// Flags if a player is in check
	bool blackInCheck, whiteInCheck;

	// Flag if draw has been reached
	bool draw;

	// Flag controls which player may move
	bool whiteTurn;

	// Integer tracks how many moves have passed since a pawn move or capture.
	int movesSinceCapture;

	// Integers hold how many pieces each color has
	int blackPieces, whitePieces;

	// Holds a copy of the previous board position
	piece previousPosition[8][8];

	// ----- Private Methods ----- \\

	// Evaluates the current board and updates pieces / flags
	void EvaluateBoard();

	// Checks if a pair of coordinates are within the bounds of a chess board
	// Takes an x coordinate and a y coordinate
	// Returns true if x, y pair reside inside the game board
	bool InBounds(const unsigned int xCoord, const unsigned int yCoord) const
	{ return xCoord < 8 && yCoord < 8; }

	/* ----- KEY OF VALUES ----- */
	// + -> White
	// - -> Black
	// 
	// 1. Pawn
	// 2. En Passant Pawn
	// 3. Rook
	// 4. Castleable Rook
	// 5. Knight
	// 6. Bishop
	// 7. Queen
	// 8. King
	// 9. Castleable King

	/* ----- Chess Special Rules ----- */
	// Castling -				King moves two tiles towards rook if both have line of sight, and neither has moved.
	//							Rook jumps over king to the first tile king moved over.
	// 
	// En Passant -				When a pawn moves two tiles, if an enemy pawn would have been able to take the pawn
	//							should it have moved one tile instead, the enemy pawn is allowed to take as if the
	//							pawn moved one tile instead of two.
	// 
	// Promotion -				When a pawn reaches the opponent's back rank, it may be promoted to any piece besides
	//							a king. 
	//							{For simplicity, we'll assume both players will only choose to promote a pawn to a queen}
	// 
	// Fifty Move Rule -		If each player has taken 50 moves (totaling 100) without capture or a pawn move, it is
	//							declared a draw. Neither player wins.
};