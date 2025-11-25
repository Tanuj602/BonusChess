#include "RookPiece.hh"
#include "ChessBoard.hh"

using Student::RookPiece;
using Student::ChessBoard;

RookPiece::RookPiece(ChessBoard &b, Color c, int r, int col)
  : ChessPiece(b, c, r, col)
{
  type = Rook;
}

bool RookPiece::canMoveToLocation(int toRow, int toColumn)
{
  // shape-only: horizontal or vertical and not staying put
  return (toRow == row && toColumn != column) ||
         (toColumn == column && toRow != row);
}

const char *RookPiece::toString()
{
  // White: ♖, Black: ♜
  return (color == White) ? "♖" : "♜";
}
