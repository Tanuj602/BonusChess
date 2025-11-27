#include "ChessBoard.hh"
#include "PawnPiece.hh"
#include "RookPiece.hh"
#include "BishopPiece.hh"
#include "KingPiece.hh"
#include "KnightPiece.hh"
#include "QueenPiece.hh"
#include <sstream>
#include <vector>
#include <cmath>

using Student::ChessBoard;
using Student::ChessPiece;
using Student::PawnPiece;
using Student::RookPiece;
using Student::BishopPiece;
using Student::KingPiece;
using Student::KnightPiece;
using Student::QueenPiece;

ChessBoard::ChessBoard(int numRow, int numCol)
{
    numRows = numRow;
    numCols = numCol;
    turn = White;
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

void ChessBoard::createChessPiece(Color col, Type ty, int startRow, int startColumn)
{
    if (board.at(startRow).at(startColumn) != nullptr) {
        delete board.at(startRow).at(startColumn);
        board.at(startRow).at(startColumn) = nullptr;
    }

    ChessPiece* p = nullptr;
    if (ty == Pawn)        p = new PawnPiece(*this, col, startRow, startColumn);
    else if (ty == Rook)   p = new RookPiece(*this, col, startRow, startColumn);
    else if (ty == Bishop) p = new BishopPiece(*this, col, startRow, startColumn);
    else if (ty == King)   p = new KingPiece(*this, col, startRow, startColumn);
    else if (ty == Knight) p = new KnightPiece(*this, col, startRow, startColumn);
    else if (ty == Queen)  p = new QueenPiece(*this, col, startRow, startColumn);
    
    board.at(startRow).at(startColumn) = p;
}

static bool in_bounds(int r, int c, int R, int C) {
    return r >= 0 && r < R && c >= 0 && c < C;
}

bool ChessBoard::isPseudoValidMove(int fromRow, int fromColumn, int toRow, int toColumn)
{
    if (!in_bounds(fromRow, fromColumn, numRows, numCols)) return false;
    if (!in_bounds(toRow, toColumn, numRows, numCols)) return false;

    ChessPiece* piece = board.at(fromRow).at(fromColumn);
    if (piece == nullptr) return false;
    if (fromRow == toRow && fromColumn == toColumn) return false;

    ChessPiece* dst = board.at(toRow).at(toColumn);
    if (dst != nullptr && dst->getColor() == piece->getColor()) return false;

    if (!piece->canMoveToLocation(toRow, toColumn)) return false;

    Type ty = piece->getType();
    if (ty == Rook || (ty == Queen && (fromRow == toRow || fromColumn == toColumn))) {
        int dr = (toRow > fromRow) ? 1 : (toRow < fromRow ? -1 : 0);
        int dc = (toColumn > fromColumn) ? 1 : (toColumn < fromColumn ? -1 : 0);
        int r = fromRow + dr;
        int c = fromColumn + dc;
        while (r != toRow || c != toColumn) {
            if (board.at(r).at(c) != nullptr) return false;
            r += dr; c += dc;
        }
    } else if (ty == Bishop || (ty == Queen && (fromRow != toRow && fromColumn != toColumn))) {
        int dr = (toRow > fromRow) ? 1 : -1;
        int dc = (toColumn > fromColumn) ? 1 : -1;
        int r = fromRow + dr;
        int c = fromColumn + dc;
        while (r != toRow) {
            if (board.at(r).at(c) != nullptr) return false;
            r += dr; c += dc;
        }
    } 

    return true;
}

bool ChessBoard::isSquareUnderAttack(int row, int column, Color byColor)
{
    for (int r = 0; r < numRows; ++r) {
        for (int c = 0; c < numCols; ++c) {
            ChessPiece* attacker = board.at(r).at(c);
            if (!attacker) continue;
            if (attacker->getColor() != byColor) continue;

            // --- CRITICAL FIX: MANUALLY CHECK PAWN THREATS ---
            // Pawns attack diagonals even if the square is empty.
            // isPseudoValidMove returns false for empty diagonals, so we fail to see the threat.
            if (attacker->getType() == Pawn) {
                int dir = (byColor == Black) ? 1 : -1;
                // Check if the pawn at (r,c) attacks (row, column)
                if (r + dir == row && (c - 1 == column || c + 1 == column)) {
                    return true;
                }
            } 
            // --- STANDARD CHECK FOR OTHER PIECES ---
            else {
                if (isPseudoValidMove(r, c, row, column)) {
                    return true;
                }
            }
        }
    }
    return false;
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

bool ChessBoard::wouldLeaveKingInCheck(int fromRow, int fromColumn, int toRow, int toColumn)
{
    ChessPiece* mover    = board.at(fromRow).at(fromColumn);
    ChessPiece* captured = board.at(toRow).at(toColumn);
    Color moverColor     = mover->getColor();
    Color enemyColor     = (moverColor == White ? Black : White);

    // En Passant Handle
    bool isEnPassant = (mover->getType() == Pawn && captured == nullptr && fromColumn != toColumn);
    ChessPiece* epVictim = nullptr;
    if (isEnPassant) {
        epVictim = board.at(fromRow).at(toColumn);
        board.at(fromRow).at(toColumn) = nullptr;
    }

    board.at(toRow).at(toColumn) = mover;
    board.at(fromRow).at(fromColumn) = nullptr;
    int oldR = mover->getRow(), oldC = mover->getColumn();
    mover->setPosition(toRow, toColumn);

    std::pair<int,int> kpos = (mover->getType() == King) ? std::pair<int,int>{toRow, toColumn} : findKing(moverColor);
    bool inCheck = false;
    if (kpos.first != -1) {
        inCheck = isSquareUnderAttack(kpos.first, kpos.second, enemyColor);
    }

    mover->setPosition(oldR, oldC);
    board.at(fromRow).at(fromColumn) = mover;
    board.at(toRow).at(toColumn) = captured;

    if (isEnPassant) {
        board.at(fromRow).at(toColumn) = epVictim;
    }

    return inCheck;
}

bool ChessBoard::isValidCastling(int fromRow, int fromColumn, int toRow, int toColumn)
{
    ChessPiece* king = board.at(fromRow).at(fromColumn);
    if (king->getHasMoved()) return false;

    Color enemy = (king->getColor() == White ? Black : White);
    if (isSquareUnderAttack(fromRow, fromColumn, enemy)) return false;

    int rookCol = (toColumn > fromColumn) ? (numCols - 1) : 0;
    ChessPiece* rook = board.at(fromRow).at(rookCol);

    if (!rook || rook->getType() != Rook || rook->getColor() != king->getColor() || rook->getHasMoved()) {
        return false;
    }

    int dir = (toColumn > fromColumn) ? 1 : -1;
    // Check path obstruction
    for (int c = fromColumn + dir; c != rookCol; c += dir) {
        if (board.at(fromRow).at(c) != nullptr) return false;
    }

    // Check path safety (skip square and dest square)
    if (isSquareUnderAttack(fromRow, fromColumn + dir, enemy)) return false;
    if (isSquareUnderAttack(toRow, toColumn, enemy)) return false;

    return true;
}

bool ChessBoard::isValidMove(int fromRow, int fromColumn, int toRow, int toColumn)
{
    if (!in_bounds(fromRow, fromColumn, numRows, numCols) || !in_bounds(toRow, toColumn, numRows, numCols)) return false;
    
    ChessPiece* piece = board.at(fromRow).at(fromColumn);
    if (!piece) return false;

    if (piece->getType() == King && std::abs(toColumn - fromColumn) == 2 && fromRow == toRow) {
        return isValidCastling(fromRow, fromColumn, toRow, toColumn);
    }

    if (!isPseudoValidMove(fromRow, fromColumn, toRow, toColumn)) return false;
    if (wouldLeaveKingInCheck(fromRow, fromColumn, toRow, toColumn)) return false;

    return true;
}

bool ChessBoard::movePiece(int fromRow, int fromColumn, int toRow, int toColumn)
{
    if (!isValidMove(fromRow, fromColumn, toRow, toColumn)) return false;
    if (board.at(fromRow).at(fromColumn)->getColor() != turn) return false;

    ChessPiece* piece = board.at(fromRow).at(fromColumn);

    // Castling
    if (piece->getType() == King && std::abs(toColumn - fromColumn) == 2) {
        int rookCol = (toColumn > fromColumn) ? (numCols - 1) : 0;
        int rookDest = (toColumn > fromColumn) ? (toColumn - 1) : (toColumn + 1);
        ChessPiece* rook = board.at(fromRow).at(rookCol);
        if (rook) {
            board.at(fromRow).at(rookDest) = rook;
            board.at(fromRow).at(rookCol) = nullptr;
            rook->setPosition(fromRow, rookDest);
            rook->markAsMoved();
        }
    }
    // En Passant
    else if (piece->getType() == Pawn && fromColumn != toColumn && board.at(toRow).at(toColumn) == nullptr) {
        ChessPiece* victim = board.at(fromRow).at(toColumn);
        if (victim) {
            delete victim;
            board.at(fromRow).at(toColumn) = nullptr;
        }
    }

    // Capture/Move
    if (board.at(toRow).at(toColumn)) {
        delete board.at(toRow).at(toColumn);
    }
    board.at(toRow).at(toColumn) = piece;
    board.at(fromRow).at(fromColumn) = nullptr;
    piece->setPosition(toRow, toColumn);

    // State Update
    if (piece->getType() == Pawn && std::abs(toRow - fromRow) == 2) {
        enPassantTarget = {(fromRow + toRow) / 2, toColumn};
    } else {
        enPassantTarget = {-1, -1};
    }
    piece->markAsMoved();

    // Promotion
    if (piece->getType() == Pawn) {
        bool promote = (piece->getColor() == White && toRow == 0) || (piece->getColor() == Black && toRow == numRows - 1);
        if (promote) {
            Color c = piece->getColor();
            delete board.at(toRow).at(toColumn);
            board.at(toRow).at(toColumn) = nullptr;
            createChessPiece(c, Queen, toRow, toColumn);
        }
    }

    turn = (turn == White ? Black : White);
    return true;
}

bool ChessBoard::isPieceUnderThreat(int row, int column) {
    if (!in_bounds(row, column, numRows, numCols)) return false;
    ChessPiece* p = board.at(row).at(column);
    if (!p) return false;
    Color enemy = (p->getColor() == White ? Black : White);
    return isSquareUnderAttack(row, column, enemy);
}

// ----------------------------------------------------------------------------
// SCORING
// ----------------------------------------------------------------------------

int ChessBoard::getPieceValue(Type t) {
    switch (t) {
        case King:   return 200;
        case Queen:  return 9;
        case Rook:   return 5;
        case Bishop: return 3;
        case Knight: return 3;
        case Pawn:   return 1;
        default:     return 0;
    }
}

float ChessBoard::scoreBoard() {
    float myScore = 0.0;
    float enemyScore = 0.0;
    Color myColor = turn;

    for (int r = 0; r < numRows; ++r) {
        for (int c = 0; c < numCols; ++c) {
            ChessPiece* p = board.at(r).at(c);
            if (!p) continue;

            float val = getPieceValue(p->getType());
            int moveCount = 0;

            for (int tr = 0; tr < numRows; ++tr) {
                for (int tc = 0; tc < numCols; ++tc) {
                    // Check standard moves
                    // Note: King castling will return false in pseudo-valid now, avoiding double count.
                    if (isPseudoValidMove(r, c, tr, tc)) {
                        if (!wouldLeaveKingInCheck(r, c, tr, tc)) {
                            moveCount++;
                        }
                    }
                    // Explicit Castling check
                    if (p->getType() == King && std::abs(tc - c) == 2 && tr == r) {
                         if (isValidCastling(r, c, tr, tc)) {
                             moveCount++;
                         }
                    }
                }
            }
            
            val += (0.1f * moveCount);

            if (p->getColor() == myColor) myScore += val;
            else enemyScore += val;
        }
    }
    return myScore - enemyScore;
}

float ChessBoard::getHighestNextScore() {
    float maxScore = -100000.0f;
    bool moveFound = false;

    for (int r = 0; r < numRows; ++r) {
        for (int c = 0; c < numCols; ++c) {
            ChessPiece* p = board.at(r).at(c);
            if (!p || p->getColor() != turn) continue;

            for (int tr = 0; tr < numRows; ++tr) {
                for (int tc = 0; tc < numCols; ++tc) {
                    if (isValidMove(r, c, tr, tc)) {
                        moveFound = true;

                        // SIMULATION
                        ChessPiece* victim = board.at(tr).at(tc);
                        ChessPiece* enPassantVictim = nullptr;
                        int epRow = r; 
                        int epCol = tc;

                        bool isEnPassant = (p->getType() == Pawn && victim == nullptr && c != tc);
                        if (isEnPassant) {
                            enPassantVictim = board.at(epRow).at(epCol);
                            board.at(epRow).at(epCol) = nullptr;
                        }

                        board.at(tr).at(tc) = p;
                        board.at(r).at(c) = nullptr;
                        p->setPosition(tr, tc);

                        bool isPromotion = (p->getType() == Pawn && (tr == 0 || tr == numRows - 1));
                        ChessPiece* promotedPawn = nullptr;
                        if (isPromotion) {
                            promotedPawn = p;
                            board.at(tr).at(tc) = new QueenPiece(*this, p->getColor(), tr, tc);
                        }

                        bool isCastling = (p->getType() == King && std::abs(tc - c) == 2);
                        ChessPiece* castleRook = nullptr;
                        int rookStartCol = -1, rookEndCol = -1;
                        if (isCastling) {
                            rookStartCol = (tc > c) ? (numCols - 1) : 0;
                            rookEndCol = (tc > c) ? (tc - 1) : (tc + 1);
                            castleRook = board.at(r).at(rookStartCol);
                            if (castleRook) {
                                board.at(r).at(rookEndCol) = castleRook;
                                board.at(r).at(rookStartCol) = nullptr;
                                castleRook->setPosition(r, rookEndCol);
                            }
                        }

                        // SCORE
                        float currentScore = scoreBoard();
                        if (currentScore > maxScore) maxScore = currentScore;

                        // UNDO
                        if (isCastling && castleRook) {
                            castleRook->setPosition(r, rookStartCol);
                            board.at(r).at(rookStartCol) = castleRook;
                            board.at(r).at(rookEndCol) = nullptr;
                        }
                        if (isPromotion) {
                            delete board.at(tr).at(tc);
                            board.at(tr).at(tc) = promotedPawn;
                        }

                        p->setPosition(r, c);
                        board.at(r).at(c) = p;
                        board.at(tr).at(tc) = victim;

                        if (isEnPassant && enPassantVictim) {
                             board.at(epRow).at(epCol) = enPassantVictim;
                        }
                    }
                }
            }
        }
    }

    if (!moveFound) return scoreBoard();
    return maxScore;
}

std::ostringstream ChessBoard::displayBoard()
{
    std::ostringstream outputString;
    outputString << "  ";
    for (int i = 0; i < numCols; i++) outputString << i << " ";
    outputString << std::endl << "  ";
    for (int i = 0; i < numCols; i++) outputString << "– ";
    outputString << std::endl;

    for (int row = 0; row < numRows; row++){
        outputString << row << "|";
        for (int column = 0; column < numCols; column++){
            ChessPiece *piece = board.at(row).at(column);
            outputString << (piece == nullptr ? " " : piece->toString()) << " ";
        }
        outputString << "|" << std::endl;
    }
    outputString << "  ";
    for (int i = 0; i < numCols; i++) outputString << "– ";
    outputString << std::endl << std::endl;
    return outputString;
}
