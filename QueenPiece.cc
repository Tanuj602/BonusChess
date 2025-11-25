#include "QueenPiece.hh"
#include "ChessBoard.hh"
#include <cmath> // Needed for std::abs

using Student::QueenPiece;
using Student::ChessBoard;
using Student::ChessPiece;

QueenPiece::QueenPiece(ChessBoard &b, Color c, int r, int col)
  : ChessPiece(b, c, r, col)
{
  type = Queen;
}

bool QueenPiece::canMoveToLocation(int toRow, int toColumn)
{
  int dr = toRow - row;
  int dc = toColumn - column;

  // 1. Cannot stay in the same place
  if (dr == 0 && dc == 0) return false;

  // 2. Determine if the move is valid geometrically
  // Queen moves either like a Rook (straight) or a Bishop (diagonal)
  bool isStraight = (dr == 0 || dc == 0);
  bool isDiagonal = (std::abs(dr) == std::abs(dc));

  // If it's neither straight nor diagonal, it's an invalid move for a Queen
  if (!isStraight && !isDiagonal) return false;

  // 3. Check for obstructions along the path
  // Determine direction of movement (-1, 0, or 1)
  int rowStep = (dr == 0) ? 0 : (dr > 0 ? 1 : -1);
  int colStep = (dc == 0) ? 0 : (dc > 0 ? 1 : -1);

  int checkRow = row + rowStep;
  int checkCol = column + colStep;

  // Loop through all squares between start and end (exclusive of end)
  while (checkRow != toRow || checkCol != toColumn) {
    if (board->getPiece(checkRow, checkCol) != nullptr) {
      return false; // Path is blocked
    }
    checkRow += rowStep;
    checkCol += colStep;
  }

  // 4. Check the destination square
  ChessPiece* target = board->getPiece(toRow, toColumn);
  
  // If empty, move is valid
  if (target == nullptr) return true;

  // If occupied, can only capture if it's the opponent's color
  if (target->getColor() != color) return true;

  // If occupied by same color, invalid
  return false;
}

const char *QueenPiece::toString()
{
  // White: ♕, Black: ♛
  return (color == White) ? "♕" : "♛";
}
