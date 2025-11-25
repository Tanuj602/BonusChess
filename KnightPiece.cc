#include "KnightPiece.hh"
#include "ChessBoard.hh"
#include <cmath> // Required for std::abs

using Student::KnightPiece;
using Student::ChessBoard;
using Student::ChessPiece;

KnightPiece::KnightPiece(ChessBoard &b, Color c, int r, int col)
  : ChessPiece(b, c, r, col)
{
  type = Knight;
}

bool KnightPiece::canMoveToLocation(int toRow, int toColumn)
{
  // Calculate the absolute difference in rows and columns
  int dr = std::abs(toRow - row);
  int dc = std::abs(toColumn - column);

  // A knight moves in an 'L' shape:
  // 2 squares one way, 1 square the other.
  // Note: We do not check for obstructions because Knights can jump.
  if ((dr == 2 && dc == 1) || (dr == 1 && dc == 2)) {
      return true;
  }
  
  return false;
}

const char *KnightPiece::toString()
{
  // White Knight: ♘, Black Knight: ♞
  return (color == White) ? "♘" : "♞";
}
