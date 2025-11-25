#include "ChessBoard.hh"
#include "PawnPiece.hh"
#include <iostream>

namespace Student
{
    /**
     * @brief
     * Performs the move if the move is valid.
     * Accounts for the turn, staying within bounds, and validity of the move.
     * Handles Pawn Promotion (Part 3).
     */
    bool ChessBoard::movePiece(int fromRow, int fromColumn, int toRow, int toColumn) {
        // ---------------------------------------------------
        // 1. INITIAL CHECKS
        // ---------------------------------------------------

        // Check 1: Bounds check (basic safety, though isValidMove usually handles this)
        if (fromRow < 0 || fromRow >= numRows || fromColumn < 0 || fromColumn >= numCols ||
            toRow < 0 || toRow >= numRows || toColumn < 0 || toColumn >= numCols) {
            return false;
        }

        // Check 2: Existence and Turn
        ChessPiece* pieceToMove = getPiece(fromRow, fromColumn);
        if (pieceToMove == nullptr) {
            return false;
        }
        
        // Ensure the piece belongs to the player whose turn it currently is
        if (pieceToMove->getColor() != turn) {
            return false;
        }

        // Check 3: Validity according to Chess Rules
        // (This function is assumed to be implemented elsewhere in Part 1/2)
        if (!isValidMove(fromRow, fromColumn, toRow, toColumn)) {
            return false;
        }

        // ---------------------------------------------------
        // 2. EXECUTE MOVE & CAPTURE
        // ---------------------------------------------------

        // If there is an opponent piece at the destination, capture (delete) it.
        if (board[toRow][toColumn] != nullptr) {
            delete board[toRow][toColumn];
            board[toRow][toColumn] = nullptr;
        }

        // Move the pointer: Place piece in new slot
        board[toRow][toColumn] = board[fromRow][fromColumn];
        
        // Clear the old slot
        board[fromRow][fromColumn] = nullptr;

        // Update the piece's internal position (if your ChessPiece class supports this)
        board[toRow][toColumn]->setPosition(toRow, toColumn);

        // ---------------------------------------------------
        // 3. PAWN PROMOTION (PART 3 LOGIC)
        // ---------------------------------------------------
        
        // Get the piece at the new location
        ChessPiece* movedPiece = board[toRow][toColumn];

        // Check if the moved piece is a Pawn using dynamic_cast.
        // This returns nullptr if the piece is NOT a Pawn.
        PawnPiece* pawn = dynamic_cast<PawnPiece*>(movedPiece);

        if (pawn != nullptr) {
            bool promote = false;

            // Condition A: White Pawn reaches the top (Row 0)
            if (movedPiece->getColor() == White && toRow == 0) {
                promote = true;
            }
            // Condition B: Black Pawn reaches the bottom (Row numRows - 1)
            else if (movedPiece->getColor() == Black && toRow == numRows - 1) {
                promote = true;
            }

            // If promotion condition is met, swap Pawn for Queen
            if (promote) {
                // Note: createChessPiece must handle deleting the Pawn currently 
                // at [toRow][toColumn] before placing the new Queen.
                createChessPiece(movedPiece->getColor(), Queen, toRow, toColumn);
            }
        }

        // ---------------------------------------------------
        // 4. FINALIZE TURN
        // ---------------------------------------------------

        // Switch turn to the other player
        if (turn == White) {
            turn = Black;
        } else {
            turn = White;
        }

        return true;
    }
}
