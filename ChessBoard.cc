#include "ChessBoard.hh"
#include "PawnPiece.hh"
#include "RookPiece.hh"
#include "BishopPiece.hh"
#include "KingPiece.hh"

using Student::ChessBoard;
using Student::ChessPiece;
using Student::PawnPiece;
using Student::RookPiece;
using Student::BishopPiece;
using Student::KingPiece;

#include <sstream>

ChessBoard::ChessBoard(int numRow, int numCol)
{
    numRows = numRow;
    numCols = numCol;
    turn = White;
    board = std::vector<std::vector<ChessPiece *>>(numRows, std::vector<ChessPiece *>(numCols, nullptr));
}

ChessBoard::~ChessBoard() {
    for (auto& rowVec : board) {
        for (ChessPiece*& p : rowVec) {
            delete p;
            p = nullptr;
        }
    }
}

void ChessBoard::createChessPiece(Color col, Type ty, int startRow, int startColumn)
{
    // clean up existing piece if any
    if (board.at(startRow).at(startColumn) != nullptr) {
        delete board.at(startRow).at(startColumn);
        board.at(startRow).at(startColumn) = nullptr;
    }

    ChessPiece* p = nullptr;
    if (ty == Pawn)   p = new PawnPiece(*this, col, startRow, startColumn);
    else if (ty == Rook)   p = new RookPiece(*this, col, startRow, startColumn);
    else if (ty == Bishop) p = new BishopPiece(*this, col, startRow, startColumn);
    else if (ty == King)   p = new KingPiece(*this, col, startRow, startColumn);
    board.at(startRow).at(startColumn) = p;
}

static bool in_bounds(int r, int c, int R, int C) {
    return r >= 0 && r < R && c >= 0 && c < C;
}


bool ChessBoard::isValidMove(int fromRow, int fromColumn, int toRow, int toColumn)
{
    // first, normal chess rules (no turns, no king safety)
    if (!isPseudoValidMove(fromRow, fromColumn, toRow, toColumn)) return false;

    // then, Part 3: the move must not leave our own king in check
    if (wouldLeaveKingInCheck(fromRow, fromColumn, toRow, toColumn)) return false;

    return true;
}

bool ChessBoard::isPseudoValidMove(int fromRow, int fromColumn, int toRow, int toColumn)
{
    // bounds
    if (!in_bounds(fromRow, fromColumn, numRows, numCols)) return false;
    if (!in_bounds(toRow, toColumn, numRows, numCols)) return false;

    ChessPiece* piece = board.at(fromRow).at(fromColumn);
    if (piece == nullptr) return false;
    if (fromRow == toRow && fromColumn == toColumn) return false;

    // destination has same color?
    ChessPiece* dst = board.at(toRow).at(toColumn);
    if (dst != nullptr && dst->getColor() == piece->getColor()) return false;

    // shape-specific rules (and pawn occupancy rules) via piece API
    if (!piece->canMoveToLocation(toRow, toColumn)) return false;

    // path obstruction for sliding pieces
    Type ty = piece->getType();
    if (ty == Rook) {
        // step along row or column
        int dr = (toRow > fromRow) ? 1 : (toRow < fromRow ? -1 : 0);
        int dc = (toColumn > fromColumn) ? 1 : (toColumn < fromColumn ? -1 : 0);
        int r = fromRow + dr;
        int c = fromColumn + dc;
        while (r != toRow || c != toColumn) {
            if (board.at(r).at(c) != nullptr) return false;
            r += dr; c += dc;
        }
    } else if (ty == Bishop) {
        int dr = (toRow > fromRow) ? 1 : -1;
        int dc = (toColumn > fromColumn) ? 1 : -1;
        int r = fromRow + dr;
        int c = fromColumn + dc;
        while (r != toRow) {
            if (board.at(r).at(c) != nullptr) return false;
            r += dr; c += dc;
        }
    } else if (ty == Pawn) {
        // Pawn path handled inside PawnPiece::canMoveToLocation (including 2-step mid cell)
    }

    return true;
}

bool ChessBoard::movePiece(int fromRow, int fromColumn, int toRow, int toColumn) {
    // ---------------------------------------------------
    // 1. INITIAL CHECKS
    // ---------------------------------------------------

    // Check 1: Bounds check
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

    // Update the piece's internal position
    board[toRow][toColumn]->setPosition(toRow, toColumn);

    // ---------------------------------------------------
    // 3. PAWN PROMOTION
    // ---------------------------------------------------
    
    // Get the piece at the new location
    ChessPiece* movedPiece = board[toRow][toColumn];

    // Check if the moved piece is a Pawn using dynamic_cast.
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
            // Remove the pawn from the board (but don't delete the pointer yet, createChessPiece might need the slot clear or overwrite it)
            // Actually, safer to rely on createChessPiece or manual swap:
            Color c = movedPiece->getColor();
            
            // Delete the pawn object
            delete board[toRow][toColumn];
            board[toRow][toColumn] = nullptr; // Safety

            // Create the new Queen at that spot
            createChessPiece(c, Queen, toRow, toColumn);
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

bool ChessBoard::isPieceUnderThreat(int row, int column)
{
    if (!in_bounds(row, column, numRows, numCols)) return false;

    ChessPiece* target = board.at(row).at(column);
    if (!target) return false; // empty square isn't "under threat" in Part 2’s definition

    Color defender = target->getColor();
    Color attackerColor = (defender == White ? Black : White);
    return isSquareUnderAttack(row, column, attackerColor);
}

std::pair<int,int> ChessBoard::findKing(Color c)
{
    for (int r = 0; r < numRows; ++r)
        for (int col = 0; col < numCols; ++col) {
            ChessPiece* p = board.at(r).at(col);
            if (p && p->getColor() == c && p->getType() == King)
                return {r, col};
        }
    return {-1, -1};
}

bool ChessBoard::isSquareUnderAttack(int row, int column, Color byColor)
{
    for (int r = 0; r < numRows; ++r) {
        for (int c = 0; c < numCols; ++c) {
            ChessPiece* attacker = board.at(r).at(c);
            if (!attacker) continue;
            if (attacker->getColor() != byColor) continue;
            if (isPseudoValidMove(r, c, row, column)) {
                return true;
            }
        }
    }
    return false;
}

bool ChessBoard::wouldLeaveKingInCheck(int fromRow, int fromColumn, int toRow, int toColumn)
{
    ChessPiece* mover    = board.at(fromRow).at(fromColumn);
    ChessPiece* captured = board.at(toRow).at(toColumn);
    Color moverColor     = mover->getColor();
    Color enemyColor     = (moverColor == White ? Black : White);

    // simulate move
    board.at(toRow).at(toColumn) = mover;
    board.at(fromRow).at(fromColumn) = nullptr;

    int oldR = mover->getRow(), oldC = mover->getColumn();
    mover->setPosition(toRow, toColumn);

    // locate my king after the move
    std::pair<int,int> kpos = (mover->getType() == King)
        ? std::pair<int,int>{toRow, toColumn}
        : findKing(moverColor);

    bool inCheck = false;
    if (kpos.first != -1) {
        inCheck = isSquareUnderAttack(kpos.first, kpos.second, enemyColor);
    }

    // revert
    mover->setPosition(oldR, oldC);
    board.at(fromRow).at(fromColumn) = mover;
    board.at(toRow).at(toColumn) = captured;

    return inCheck;
}

std::ostringstream ChessBoard::displayBoard()
{
    std::ostringstream outputString;
    // top scale
    outputString << "  ";
    for (int i = 0; i < numCols; i++){
        outputString << i << " ";
    }
    outputString << std::endl << "  ";
    // top border
    for (int i = 0; i < numCols; i++){
        outputString << "– ";
    }
    outputString << std::endl;

    for (int row = 0; row < numRows; row++){
        outputString << row << "|";
        for (int column = 0; column < numCols; column++){
            ChessPiece *piece = board.at(row).at(column);
            outputString << (piece == nullptr ? " " : piece->toString()) << " ";
        }
        outputString << "|" << std::endl;
    }

    // bottom border
    outputString << "  ";
    for (int i = 0; i < numCols; i++){
        outputString << "– ";
    }
    outputString << std::endl << std::endl;

    return outputString;
}
