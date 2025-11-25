# compile
g++ -std=c++17 -O2 -Wall main.cc ChessBoard.cc ChessPiece.cc PawnPiece.cc RookPiece.cc BishopPiece.cc KingPiece.cc -o chess_part1

# single file
./chess_part1 test_files/part1_4x4_1.txt

# suite default ranges
./chess_part1 test_files 4x4
./chess_part1 test_files 6x6
./chess_part1 test_files 8x8

# suite with custom range
./chess_part1 test_files 8x8 10 19
