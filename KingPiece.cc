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
    
    // Standard move
    if (std::abs(dr) <= 1 && std::abs(dc) <= 1) {
        return !(dr == 0 && dc == 0);
    }
    
    // Castling move (Geometry only): 
    // King moves 2 squares horizontally, same row.
    if (dr == 0 && std::abs(dc) == 2) {
        return true;
    }

    return false;
}

const char *KingPiece::toString()
{
    // White: ♔, Black: ♚
    return (color == White) ? "♔" : "♚";
}
