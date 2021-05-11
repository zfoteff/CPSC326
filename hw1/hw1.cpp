//----------------------------------------------------------------------
// NAME:  Zac Foteff
// FILE:  hw1.cpp
// DATE:  01-26-2021
// DESC:  Simple test program for creating and printing MyPL tokens.
//----------------------------------------------------------------------

#include <iostream>
#include "token.h"


using namespace std;


int main()
{
  // basic symbols
  cout << Token(ASSIGN, "=", 1, 1).to_string() << endl;
  cout << Token(COMMA, ",", 1, 2).to_string() << endl;
  cout << Token(DOT, ".", 1, 3).to_string() << endl;
  cout << Token(LPAREN, "(", 1, 4).to_string() << endl;
  cout << Token(RPAREN, ")", 1, 5).to_string() << endl;
  cout << Token(COLON, ":", 1, 6).to_string() << endl;
  // math operators
  cout << Token(PLUS, ":", 2, 1).to_string() << endl;
  cout << Token(MINUS, ":", 2, 2).to_string() << endl;
  cout << Token(MULTIPLY, ":", 2, 3).to_string() << endl;
  cout << Token(DIVIDE, ":", 2, 4).to_string() << endl;
  cout << Token(MODULO, ":", 2, 5).to_string() << endl;
  cout << Token(NEG, ":", 2, 6).to_string() << endl;
  // logical operators
  cout << Token(AND, "and", 3, 1).to_string() << endl;
  cout << Token(OR, "or", 3, 5).to_string() << endl;
  cout << Token(NOT, "not", 3, 8).to_string() << endl;
  // comparators
  cout << Token(EQUAL, "=", 4, 1).to_string() << endl;
  cout << Token(GREATER, ">", 4, 4).to_string() << endl;
  cout << Token(GREATER_EQUAL, ">=", 4, 6).to_string() << endl;
  cout << Token(LESS, "<", 4, 9).to_string() << endl;
  cout << Token(LESS_EQUAL, "<=", 4, 11).to_string() << endl;
  cout << Token(NOT_EQUAL, "!=", 4, 14).to_string() << endl;
  // reserved words
  cout << Token(TYPE, "type", 5, 1).to_string() << endl;
  cout << Token(WHILE, "while", 5, 6).to_string() << endl;
  cout << Token(FOR, "for", 5, 12).to_string() << endl;
  cout << Token(TO, "to", 5, 16).to_string() << endl;
  cout << Token(DO, "do", 5, 19).to_string() << endl;
  cout << Token(IF, "if", 5, 22).to_string() << endl;
  cout << Token(THEN, "then", 5, 25).to_string() << endl;
  cout << Token(ELSEIF, "elseif", 5, 30).to_string() << endl;
  cout << Token(ELSE, "else", 5, 37).to_string() << endl;
  cout << Token(END, "end", 5, 42).to_string() << endl;
  cout << Token(FUN, "fun", 5, 46).to_string() << endl;
  cout << Token(VAR, "var", 5, 50).to_string() << endl;
  cout << Token(RETURN, "return", 5, 54).to_string() << endl;
  cout << Token(NEW, "new", 5, 61).to_string() << endl;
  // primitive types
  cout << Token(BOOL_TYPE, "bool", 6, 1).to_string() << endl;
  cout << Token(INT_TYPE, "int", 6, 6).to_string() << endl;
  cout << Token(DOUBLE_TYPE, "double", 6, 10).to_string() << endl;
  cout << Token(CHAR_TYPE, "char", 6, 17).to_string() << endl;
  cout << Token(STRING_TYPE, "string", 6, 22).to_string() << endl;
  // values
  cout << Token(BOOL_VAL, "true", 7, 1).to_string() << endl;
  cout << Token(INT_VAL, "42", 7, 6).to_string() << endl;
  cout << Token(DOUBLE_VAL, "3.14", 7, 9).to_string() << endl;
  cout << Token(STRING_VAL, "Hello World!", 7, 14).to_string()<<endl;
  cout << Token(CHAR_VAL, "a", 7, 27).to_string() << endl;
  cout << Token(ID, "x_val", 7, 29).to_string() << endl;
  cout << Token(NIL, "nil", 7, 34).to_string() << endl;
  // end-of-stream
  cout << Token(EOS, "", 8, 1).to_string() << endl;
}
