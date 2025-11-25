// KingPiece.cc
#include "KingPiece.hh"
#include "ChessBoard.hh"

using Student::KingPiece;
using Student::ChessBoard;

KingPiece::KingPiece(ChessBoard &b, Color c, int r, int col)
  : ChessPiece(b, c, r, col)
{
    type = King;
}

bool KingPiece::canMoveToLocation(int toRow, int toColumn)
{
    int dr = toRow - row;
    int dc = toColumn - column;
    if (dr == 0 && dc == 0) return false;
    return (dr >= -1 && dr <= 1) && (dc >= -1 && dc <= 1);
}

const char *KingPiece::toString()
{
    // White: ♔, Black: ♚
    return (color == White) ? "♔" : "♚";
}
