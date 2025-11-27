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
    } 
    // Pawn path handled inside PawnPiece::canMoveToLocation

    return true;
}

bool ChessBoard::movePiece(int fromRow, int fromColumn, int toRow, int toColumn)
{
    if (!isValidMove(fromRow, fromColumn, toRow, toColumn)) return false;
    if (board.at(fromRow).at(fromColumn)->getColor() != turn) return false;

    ChessPiece* piece = board.at(fromRow).at(fromColumn);
    
    // --- EN PASSANT CAPTURE LOGIC ---
    // If a Pawn moves diagonally into an empty square, it MUST be En Passant
    // (PawnPiece::canMoveToLocation only allows this if it matches enPassantTarget)
    if (piece->getType() == Pawn && board.at(toRow).at(toColumn) == nullptr && fromColumn != toColumn) {
        // The victim pawn is located at [fromRow][toColumn]
        // (Directly "behind" where the capturing pawn lands, relative to the capture direction)
        ChessPiece* victim = board.at(fromRow).at(toColumn);
        if (victim != nullptr) {
            delete victim;
            board.at(fromRow).at(toColumn) = nullptr;
        }
    }

    // --- STANDARD CAPTURE LOGIC ---
    if (board.at(toRow).at(toColumn) != nullptr) {
        delete board.at(toRow).at(toColumn);
        board.at(toRow).at(toColumn) = nullptr;
    }

    // --- EXECUTE MOVE ---
    board.at(toRow).at(toColumn) = piece;
    board.at(fromRow).at(fromColumn) = nullptr;
    piece->setPosition(toRow, toColumn);

    // --- PAWN PROMOTION (Existing Code) ---
    PawnPiece* pawn = dynamic_cast<PawnPiece*>(piece);
    if (pawn != nullptr) {
        bool promote = false;
        if (piece->getColor() == White && toRow == 0) promote = true;
        else if (piece->getColor() == Black && toRow == numRows - 1) promote = true;

        if (promote) {
            Color c = piece->getColor();
            delete board.at(toRow).at(toColumn);
            board.at(toRow).at(toColumn) = nullptr;
            createChessPiece(c, Queen, toRow, toColumn);
            piece = board.at(toRow).at(toColumn); // Update pointer to new piece
        }
    }

    // --- UPDATE EN PASSANT TARGET FOR NEXT TURN ---
    // If this move was a Pawn moving 2 squares, mark the skipped square
    if (piece->getType() == Pawn && std::abs(toRow - fromRow) == 2) {
        // The skipped square is the average of the rows
        int skippedRow = (fromRow + toRow) / 2;
        enPassantTarget = {skippedRow, toColumn};
    } else {
        // Any other move (including single pawn steps) clears the target
        enPassantTarget = {-1, -1};
    }

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
