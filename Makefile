# Compiler and Flags
CXX      = g++
CXXFLAGS = -g -Wall -std=c++17 -pthread

# Common Source Files
# Note: PawnPromote.cc is included here because it defines movePiece.
# You MUST remove movePiece from ChessBoard.cc for this to work.
COMMON_SRCS = BishopPiece.cc \
              ChessBoard.cc \
              ChessPiece.cc \
              KingPiece.cc \
              KnightPiece.cc \
              PawnPiece.cc \
              PawnPromote.cc \
              QueenPiece.cc \
              RookPiece.cc

# Object files
COMMON_OBJS = $(COMMON_SRCS:.cc=.o)

# Targets
all: part1 part2 part3

part1: $(COMMON_OBJS) main_part1.o
	$(CXX) $(CXXFLAGS) -o chess_part1 $(COMMON_OBJS) main_part1.o

part2: $(COMMON_OBJS) main_part2.o
	$(CXX) $(CXXFLAGS) -o chess_part2 $(COMMON_OBJS) main_part2.o

part3: $(COMMON_OBJS) main_part3.o
	$(CXX) $(CXXFLAGS) -o chess_part3 $(COMMON_OBJS) main_part3.o

# Generic rule for compiling .cc to .o
%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o chess_part1 chess_part2 chess_part3