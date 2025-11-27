#include "KingPiece.hh"
#include "ChessBoard.hh"
#include <cmath>

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
    
    // Standard move: 1 step in any direction
    if (std::abs(dr) <= 1 && std::abs(dc) <= 1) {
        // Cannot stay in same spot
        return !(dr == 0 && dc == 0);
    }

    // IMPORTANT: Return false for castling (2 steps) here.
    // The Board logic will handle the special case.
    return false;
}

const char *KingPiece::toString()
{
    return (color == White) ? "♔" : "♚";
}
