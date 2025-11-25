#include "BishopPiece.hh"
#include "ChessBoard.hh"

using Student::BishopPiece;
using Student::ChessBoard;

BishopPiece::BishopPiece(ChessBoard &b, Color c, int r, int col)
  : ChessPiece(b, c, r, col)
{
  type = Bishop;
}

bool BishopPiece::canMoveToLocation(int toRow, int toColumn)
{
  int dr = toRow - row;
  int dc = toColumn - column;
  if (dr == 0 && dc == 0) return false;
  return (dr == dc) || (dr == -dc);
}

const char *BishopPiece::toString()
{
  // White: ♗, Black: ♝
  return (color == White) ? "♗" : "♝";
}
