#include "PawnPiece.hh"
#include "ChessBoard.hh"

using Student::PawnPiece;
using Student::ChessBoard;
using Student::ChessPiece;

PawnPiece::PawnPiece(ChessBoard &b, Color c, int r, int col)
  : ChessPiece(b, c, r, col)
{
  type = Pawn;
}

bool PawnPiece::canMoveToLocation(int toRow, int toColumn)
{
  // Direction: Black moves down (+1), White moves up (-1)
  int dir = (color == Black) ? 1 : -1;
  int startRow = (color == Black) ? 1 : (board->getNumRows() - 2);

  int dr = toRow - row;
  int dc = toColumn - column;

  // Cannot stay in place
  if (dr == 0 && dc == 0) return false;

  // Forward move in same column
  if (dc == 0) {
    // One step forward
    if (dr == dir) {
      if (board->getPiece(toRow, toColumn) == nullptr) return true;
      return false;
    }
    // Two steps from starting row (must be clear)
    if (row == startRow && dr == 2*dir) {
      int midRow = row + dir;
      if (board->getPiece(midRow, column) == nullptr &&
          board->getPiece(toRow, toColumn) == nullptr)
        return true;
      return false;
    }
    return false;
  }

  // Diagonal capture by one step
  if ((dc == 1 || dc == -1) && dr == dir) {
    ChessPiece* target = board->getPiece(toRow, toColumn);
    if (target != nullptr && target->getColor() != color) return true;
    return false;
  }

  return false;
}

const char *PawnPiece::toString()
{
  // White: ♙, Black: ♟
  return (color == White) ? "♙" : "♟";
}
