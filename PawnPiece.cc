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

    // 1. Forward move (no capture)
    if (dc == 0) {
        // One step
        if (dr == dir) {
            return (board->getPiece(toRow, toColumn) == nullptr);
        }
        // Two steps
        if (row == startRow && dr == 2 * dir) {
            int midRow = row + dir;
            return (board->getPiece(midRow, column) == nullptr &&
                    board->getPiece(toRow, toColumn) == nullptr);
        }
        return false;
    }

    // 2. Diagonal Capture (Normal OR En Passant)
    if (std::abs(dc) == 1 && dr == dir) {
        ChessPiece* target = board->getPiece(toRow, toColumn);

        // A) Normal Capture
        if (target != nullptr && target->getColor() != color) {
            return true;
        }

        // B) En Passant Capture
        // Target square is empty, but matches the En Passant coordinate
        std::pair<int, int> epTarget = board->getEnPassantTarget();
        if (target == nullptr && toRow == epTarget.first && toColumn == epTarget.second) {
            return true;
        }
    }

    return false;
}

const char *PawnPiece::toString()
{
  // White: ♙, Black: ♟
  return (color == White) ? "♙" : "♟";
}
