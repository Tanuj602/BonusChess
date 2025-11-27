#include "ChessBoard.hh"
#include "PawnPiece.hh"
#include "RookPiece.hh"
#include "BishopPiece.hh"
#include "KingPiece.hh"
#include "KnightPiece.hh"
#include "QueenPiece.hh"
#include <sstream>

using Student::ChessBoard;
using Student::ChessPiece;
using Student::PawnPiece;
using Student::RookPiece;
using Student::BishopPiece;
using Student::KingPiece;

ChessBoard::ChessBoard(int numRow, int numCol)
{
    numRows = numRow;
    numCols = numCol;
    turn = White;
    // Initialize target to invalid
    enPassantTarget = {-1, -1}; 
    board = std::vector<std::vector<ChessPiece *>>(numRows, std::vector<ChessPiece *>(numCols, nullptr));
}

ChessBoard::~ChessBoard() {
    for (auto& rowVec : board) {
        for (ChessPiece*& p : rowVec) {
            if (p != nullptr) {
                delete p;
                p = nullptr;
            }
        }
    }
}

// In ChessBoard.cc

void ChessBoard::createChessPiece(Color col, Type ty, int startRow, int startColumn)
{
    // clean up existing piece if any
    if (board.at(startRow).at(startColumn) != nullptr) {
        delete board.at(startRow).at(startColumn);
        board.at(startRow).at(startColumn) = nullptr;
    }

    ChessPiece* p = nullptr;
    if (ty == Pawn)        p = new PawnPiece(*this, col, startRow, startColumn);
    else if (ty == Rook)   p = new RookPiece(*this, col, startRow, startColumn);
    else if (ty == Bishop) p = new BishopPiece(*this, col, startRow, startColumn);
    else if (ty == King)   p = new KingPiece(*this, col, startRow, startColumn);
    // --- ADD THESE TWO LINES ---
    else if (ty == Knight) p = new KnightPiece(*this, col, startRow, startColumn);
    else if (ty == Queen)  p = new QueenPiece(*this, col, startRow, startColumn);
    // ---------------------------
    
    board.at(startRow).at(startColumn) = p;
}

// Helper for bounds checking
static bool in_bounds(int r, int c, int R, int C) {
    return r >= 0 && r < R && c >= 0 && c < C;
}

bool ChessBoard::isValidMove(int fromRow, int fromColumn, int toRow, int toColumn)
{
    // Bounds check
    if (!in_bounds(fromRow, fromColumn, numRows, numCols) ||
        !in_bounds(toRow, toColumn, numRows, numCols)) return false;

    ChessPiece* piece = board.at(fromRow).at(fromColumn);
    if (!piece) return false;

    // --- CASTLING CHECK ---
    // If it is a King moving 2 spaces horizontally...
    if (piece->getType() == King && 
        fromRow == toRow && 
        std::abs(toColumn - fromColumn) == 2) {
        return isValidCastling(fromRow, fromColumn, toRow, toColumn);
    }
    // ----------------------

    // Standard logic for all other moves
    if (!isPseudoValidMove(fromRow, fromColumn, toRow, toColumn)) return false;
    if (wouldLeaveKingInCheck(fromRow, fromColumn, toRow, toColumn)) return false;

    return true;
}

// In ChessBoard.cc

bool ChessBoard::isValidCastling(int fromRow, int fromColumn, int toRow, int toColumn)
{
    ChessPiece* king = board.at(fromRow).at(fromColumn);
    
    // 1. King must not have moved
    if (king->getHasMoved()) return false;

    // 2. King must not be in check currently
    if (isSquareUnderAttack(fromRow, fromColumn, (king->getColor() == White ? Black : White))) return false;

    // Identify Rook position based on direction
    int rookCol = (toColumn > fromColumn) ? 7 : 0; // Column 7 for KingSide, 0 for QueenSide
    ChessPiece* rook = board.at(fromRow).at(rookCol);

    // 3. Rook must exist, be a Rook, same color, and not have moved
    if (rook == nullptr || rook->getType() != Rook || 
        rook->getColor() != king->getColor() || rook->getHasMoved()) {
        return false;
    }

    // 4. Path must be unobstructed
    // We check the squares between King and Rook
    int direction = (toColumn > fromColumn) ? 1 : -1;
    for (int c = fromColumn + direction; c != rookCol; c += direction) {
        if (board.at(fromRow).at(c) != nullptr) return false;
    }

    // 5. The square the King skips over (and the destination) must not be under threat
    // The King moves 2 squares. We check the 'middle' square and the 'destination' square.
    int middleCol = fromColumn + direction;
    Color enemyColor = (king->getColor() == White ? Black : White);

    if (isSquareUnderAttack(fromRow, middleCol, enemyColor)) return false;
    if (isSquareUnderAttack(toRow, toColumn, enemyColor)) return false;

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
    } 
    // Pawn path handled inside PawnPiece::canMoveToLocation

    return true;
}

bool ChessBoard::movePiece(int fromRow, int fromColumn, int toRow, int toColumn)
{
    // ---------------------------------------------------
    // 1. VALIDATION
    // ---------------------------------------------------
    if (!isValidMove(fromRow, fromColumn, toRow, toColumn)) return false;
    
    ChessPiece* piece = board.at(fromRow).at(fromColumn);
    
    // Check if piece exists and is the correct turn
    if (!piece || piece->getColor() != turn) return false;

    // ---------------------------------------------------
    // 2. SPECIAL MOVE EXECUTION (Castling & En Passant)
    // ---------------------------------------------------

    // -- CASTLING --
    // If King moves 2 squares horizontally, we must also move the Rook.
    if (piece->getType() == King && std::abs(toColumn - fromColumn) == 2) {
        int rookCol = (toColumn > fromColumn) ? 7 : 0;      // Rook is at col 0 or 7
        int rookDestCol = (toColumn > fromColumn) ? 5 : 3;  // Rook lands at col 3 or 5
        
        ChessPiece* rook = board.at(fromRow).at(rookCol);
        
        // Move the Rook
        if (rook) {
            board.at(fromRow).at(rookDestCol) = rook;
            board.at(fromRow).at(rookCol) = nullptr;
            rook->setPosition(fromRow, rookDestCol);
            rook->markAsMoved();
        }
    }

    // -- EN PASSANT CAPTURE --
    // If a Pawn moves diagonally to an empty square, it is an En Passant capture.
    else if (piece->getType() == Pawn && 
             board.at(toRow).at(toColumn) == nullptr && 
             fromColumn != toColumn) {
        
        // The victim pawn is "behind" the destination square
        ChessPiece* victim = board.at(fromRow).at(toColumn);
        if (victim) {
            delete victim;
            board.at(fromRow).at(toColumn) = nullptr;
        }
    }

    // ---------------------------------------------------
    // 3. STANDARD CAPTURE & MOVE
    // ---------------------------------------------------

    // If destination is occupied (Standard Capture), remove that piece
    if (board.at(toRow).at(toColumn) != nullptr) {
        delete board.at(toRow).at(toColumn);
        board.at(toRow).at(toColumn) = nullptr;
    }

    // Move the actual piece
    board.at(toRow).at(toColumn) = piece;
    board.at(fromRow).at(fromColumn) = nullptr;
    piece->setPosition(toRow, toColumn);

    // ---------------------------------------------------
    // 4. UPDATE STATE (En Passant Target & HasMoved)
    // ---------------------------------------------------

    // Set En Passant Target if a Pawn moves 2 squares
    if (piece->getType() == Pawn && std::abs(fromRow - toRow) == 2) {
        enPassantTarget = {(fromRow + toRow) / 2, toColumn};
    } else {
        // Reset target on any other move
        enPassantTarget = {-1, -1};
    }

    // Mark piece as having moved (important for Castling logic)
    piece->markAsMoved();

    // ---------------------------------------------------
    // 5. PAWN PROMOTION
    // ---------------------------------------------------
    if (piece->getType() == Pawn) {
        bool promote = (piece->getColor() == White && toRow == 0) ||
                       (piece->getColor() == Black && toRow == numRows - 1);

        if (promote) {
            Color c = piece->getColor();
            // Delete the Pawn
            delete board.at(toRow).at(toColumn);
            board.at(toRow).at(toColumn) = nullptr;

            // Create the Queen
            createChessPiece(c, Queen, toRow, toColumn);
        }
    }

    // ---------------------------------------------------
    // 6. FINALIZE TURN
    // ---------------------------------------------------
    turn = (turn == White ? Black : White);
    return true;
}

bool ChessBoard::isPieceUnderThreat(int row, int column)
{
    if (!in_bounds(row, column, numRows, numCols)) return false;

    ChessPiece* target = board.at(row).at(column);
    if (!target) return false; 

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
