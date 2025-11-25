# Compiler and Flags
CXX      = g++
CXXFLAGS = -g -Wall -std=c++17 -pthread

# --------------------------------------------------------
# 1. Common Source Files (Logic Only)
# --------------------------------------------------------
# These are the files that contain your chess logic.
# NOTE: We do NOT include 'PawnPromote.cc' here because 
# its logic should now be inside 'ChessBoard.cc'.
COMMON_SRCS = BishopPiece.cc \
              ChessBoard.cc \
              ChessPiece.cc \
              KingPiece.cc \
              KnightPiece.cc \
              PawnPiece.cc \
              QueenPiece.cc \
              RookPiece.cc

# Convert .cc filenames to .o object filenames
COMMON_OBJS = $(COMMON_SRCS:.cc=.o)

# --------------------------------------------------------
# 2. Build Targets
# --------------------------------------------------------
# Typing 'make' will build all three executables.
all: chess_part1 chess_part2 chess_part3

# Part 1 Executable
chess_part1: $(COMMON_OBJS) main_part1.o
	$(CXX) $(CXXFLAGS) -o chess_part1 $(COMMON_OBJS) main_part1.o

# Part 2 Executable
chess_part2: $(COMMON_OBJS) main_part2.o
	$(CXX) $(CXXFLAGS) -o chess_part2 $(COMMON_OBJS) main_part2.o

# Part 3 Executable
chess_part3: $(COMMON_OBJS) main_part3.o
	$(CXX) $(CXXFLAGS) -o chess_part3 $(COMMON_OBJS) main_part3.o

# --------------------------------------------------------
# 3. Compilation Rules
# --------------------------------------------------------
# This rule tells make how to turn any .cc file into a .o file
%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up temporary files and executables
clean:
	rm -f *.o chess_part1 chess_part2 chess_part3