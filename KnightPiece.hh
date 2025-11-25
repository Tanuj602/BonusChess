#ifndef __KNIGHTPIECE_H__
#define __KNIGHTPIECE_H__

#include "ChessPiece.hh"
#include "ChessBoard.hh"

/**
 * Student implementation of a Pawn chess piece.
 * The class is defined within the Student namespace.
 */
namespace Student
{
    class KnightPiece : public ChessPiece
    {
    public:
        KnightPiece(ChessBoard &board, Color color, int row, int column);
        bool canMoveToLocation(int toRow, int toColumn) override;
        const char *toString() override;
    };
}

#endif
