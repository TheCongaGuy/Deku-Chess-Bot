#pragma once

#include <map>

// Define a piece as an 8 bit integer
typedef __int8 piece;

// Holds information about the current game state
class GameBoard
{
public:
	// Default Constructor
	GameBoard();

	// [Data Members]
private:
	// A 2D Array of pieces representing a game board; Top left is (0, 0)
	piece gameBoard[8][8] = { 0 };

	// Flags if a player is in check
	bool blackInCheck, whiteInCheck;

	// Integer tracks how many moves have passed since a pawn move or capture.
	int movesSinceCapture;

	// Map holds a series of positions as keys, and the number of times this possition has appeared as values
	std::map<GameBoard, int> previousPositions;

	// KEY OF VALUES
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

	// Chess Special Rules
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
	// Stalemate -				If a player is in check and has no legal moves, the game ends in a stalemate. Neither player 
	//							wins.
	// 
	// Threefold Repitition -	If a gamestate occurs three times in a game, it is declared a draw. Neither player wins.
	// 
	// Fifty Move Rule -		If each player has taken 50 moves (totaling 100) without capture or a pawn move, it is
	//							declared a draw. Neither player wins.
};