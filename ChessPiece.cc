#include "ChessPiece.hh"
#include "ChessBoard.hh"

using Student::ChessPiece;
using Student::ChessBoard;

ChessPiece::ChessPiece(ChessBoard &b, Color c, int r, int col)
  : board(&b), color(c), type(Pawn), row(r), column(col) {}

Color ChessPiece::getColor() { return color; }
Type  ChessPiece::getType()  { return type;  }
int   ChessPiece::getRow()   { return row;   }
int   ChessPiece::getColumn(){ return column;}

void  ChessPiece::setPosition(int r, int c) {
  row = r;
  column = c;
}
