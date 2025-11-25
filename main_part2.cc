// main_part2.cc
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "ChessBoard.hh"
#include "ChessPiece.hh"
#include "PawnPiece.hh"
#include "RookPiece.hh"
#include "BishopPiece.hh"
#include "KingPiece.hh"

using namespace Student;

static Color charToColor(char c) { return (c == 'w' || c == 'W') ? White : Black; }
static Type  charToType(char c) {
    switch (c) {
        case 'p': case 'P': return Pawn;
        case 'r': case 'R': return Rook;
        case 'b': case 'B': return Bishop;
        case 'k': case 'K': return King;
        default: return Pawn;
    }
}

static void runIsValidScan(ChessBoard &board) {
    int rows = board.getNumRows();
    int cols = board.getNumCols();
    for (int r1 = 0; r1 < rows; ++r1)
        for (int c1 = 0; c1 < cols; ++c1)
            for (int r2 = 0; r2 < rows; ++r2)
                for (int c2 = 0; c2 < cols; ++c2)
                    board.isValidMove(r1, c1, r2, c2);
}

static bool runOneConfigFile(const std::string &path) {
    std::ifstream fin(path);
    if (!fin.is_open()) {
        std::cerr << "[FAIL] open: " << path << "\n";
        return false;
    }

    int ignoreScore = 0, rows = 0, cols = 0;
    if (!(fin >> ignoreScore >> rows >> cols)) {
        std::cerr << "[FAIL] parse header in: " << path << "\n";
        return false;
    }

    ChessBoard board(rows, cols);

    // initial pieces until "~"
    std::string color, type;
    int r, c;
    while (fin >> color) {
        if (color == "~") break;
        fin >> type >> r >> c;
        board.createChessPiece(charToColor(color[0]), charToType(type[0]), r, c);
    }

    // commands
    std::string cmd;
    while (fin >> cmd) {
        if (cmd == "isValidScan") {
            runIsValidScan(board);
            std::cout << "[OK] isValidScan\n";
        } else if (cmd == "underThreatScan") {
            for (int rr = 0; rr < board.getNumRows(); ++rr)
                for (int cc = 0; cc < board.getNumCols(); ++cc)
                    board.isPieceUnderThreat(rr, cc);
            std::cout << "[OK] underThreatScan\n";
        } else if (cmd == "movePiece") {
            int fr, fc, tr, tc;
            fin >> fr >> fc >> tr >> tc;
            bool ok = board.movePiece(fr, fc, tr, tc);
            std::cout << (ok ? "[OK]" : "[FAIL]") << " movePiece\n";
        }
    }

    std::cout << board.displayBoard().str();
    return true;
}

static bool looksLikeTxtFile(const std::string &s) {
    return s.size() >= 4 && s.substr(s.size()-4) == ".txt";
}

int main(int argc, char* argv[]) {
    // Part 2 runner:
    // 1) Single file:   ./chess_part2 <path/to/config.txt>
    // 2) Suite default: ./chess_part2 <dir> <4x4|6x6|8x8>
    // 3) Suite custom:  ./chess_part2 <dir> <4x4|6x6|8x8> <start> <end>
    if (argc < 2) {
        std::cerr << "Usage:\n"
                  << "  Single file:   ./chess_part2 <path/to/config.txt>\n"
                  << "  Suite default: ./chess_part2 <dir> <4x4|6x6|8x8>\n"
                  << "  Suite custom:  ./chess_part2 <dir> <4x4|6x6|8x8> <start> <end>\n";
        return 1;
    }

    std::string arg1 = argv[1];

    // Single file mode (works for either part1/part2 style files)
    if (looksLikeTxtFile(arg1) || (argc == 2 && arg1.find(".txt") != std::string::npos)) {
        bool ok = runOneConfigFile(arg1);
        return ok ? 0 : 2;
    }

    // Suite mode (this main is intended for part2_* suites)
    if (argc < 3) {
        std::cerr << "For suite mode, provide directory and size (4x4|6x6|8x8).\n";
        return 1;
    }

    std::string dir  = argv[1];
    std::string size = argv[2];

    int start = 1, end = 1;
    if (size == "4x4")      { start = 1; end = 8;  }
    else if (size == "6x6") { start = 1; end = 8;  }
    else if (size == "8x8") { start = 1; end = 19; }
    else {
        std::cerr << "Unknown size: " << size << " (use 4x4|6x6|8x8)\n";
        return 1;
    }

    // Optional custom range
    if (argc >= 5) {
        start = std::stoi(argv[3]);
        end   = std::stoi(argv[4]);
        if (start > end) std::swap(start, end);
    }

    int failures = 0;
    for (int i = start; i <= end; ++i) {
        std::ostringstream p;
        p << dir << "/part2_" << size << "_" << i << ".txt";
        std::cout << "=== Running " << p.str() << " ===\n";
        if (!runOneConfigFile(p.str())) {
            ++failures;
        }
    }

    if (failures == 0) {
        std::cout << "\nAll tests ran to completion (no file/parse failures).\n";
    } else {
        std::cout << "\n" << failures << " test(s) failed to open/parse.\n";
    }

    return failures == 0 ? 0 : 3;
}
