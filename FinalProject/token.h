//----------------------------------------------------------------------
// NAME:  Zac Foteff
// FILE:  token.h
// DATE:  Spring 2021
// DESC:  Token representation for MyPL.
//----------------------------------------------------------------------

#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <map>


// MyPL allowable token types
enum TokenType {
  // basic symbols
  ASSIGN, COMMA, DOT, LPAREN, RPAREN, COLON,
  // math operators
  PLUS, MINUS, MULTIPLY, DIVIDE, MODULO, NEG,
  // logical operators
  AND, OR, NOT,
  // comparators
  EQUAL, GREATER, GREATER_EQUAL, LESS, LESS_EQUAL, NOT_EQUAL,
  // reserved words
  TYPE, WHILE, FOR, TO, DO, IF, THEN, ELSEIF, ELSE, END, FUN, VAR, RETURN, NEW, TRY, THROW, CATCH,
  // primitive types
  BOOL_TYPE, INT_TYPE, DOUBLE_TYPE, CHAR_TYPE, STRING_TYPE,
  // values
  BOOL_VAL, INT_VAL, DOUBLE_VAL, STRING_VAL, CHAR_VAL, ID, NIL,
  // end-of-stream
  EOS
};


class Token
{
public:

  // default constructor
  Token();

  // constructor
  Token(TokenType type, const std::string& lexeme, int line, int column);

  // return the type of the token
  TokenType type() const;

  // return the token string value
  std::string lexeme() const;

  // return the line location of lexeme
  int line() const;

  // return the column location where the lexeme starts
  int column() const;

  // a string representation of the token object
  std::string to_string() const;

private:

  // the type of the token
  TokenType token_type;

  // the token's value in the program
  std::string token_lexeme;

  // the line location of the lexeme (starts at 1)
  int token_line;

  // the column location of the start of the lexeme (starts at 1)
  int token_column;

  // token type to string representation (for printing)
  std::map<TokenType,std::string> token_type_map =
    { // basic symbols
      {ASSIGN, "ASSIGN"}, {COMMA, "COMMA"}, {DOT, "DOT"},
      {LPAREN, "LPAREN"}, {RPAREN, "RPAREN"}, {COLON, "COLON"},
      // math operators
      {PLUS, "PLUS"}, {MINUS, "MINUS"}, {MULTIPLY, "MULTIPLY"},
      {DIVIDE, "DIVIDE"}, {MODULO, "MODULO"}, {NEG, "NEG"},
      {STRING_VAL, "STRING_VAL"},
      // logical operators
      {AND, "AND"}, {OR, "OR"}, {NOT, "NOT"},
      // comparators
      {EQUAL, "EQUAL"}, {GREATER, "GREATER"},
      {GREATER_EQUAL, "GREATER_EQUAL"}, {LESS, "LESS"},
      {LESS_EQUAL, "LESS_EQUAL"}, {NOT_EQUAL, "NOT_EQUAL"},
      // reserved words
      {TYPE, "TYPE"}, {WHILE, "WHILE"}, {FOR, "FOR"}, {TO, "TO"},
      {DO, "DO"}, {IF, "IF"}, {THEN, "THEN"}, {ELSEIF, "ELSEIF"},
      {ELSE, "ELSE"}, {END, "END"}, {FUN, "FUN"}, {VAR, "VAR"},
      {RETURN, "RETURN"}, {NEW, "NEW"}, {TRY, "TRY"}, {CATCH, "CATCH"},
      {THROW,"THROW"},
      // primitive types
      {BOOL_TYPE, "BOOL_TYPE"}, {INT_TYPE, "INT_TYPE"},
      {DOUBLE_TYPE, "DOUBLE_TYPE"}, {CHAR_TYPE, "CHAR_TYPE"},
      {STRING_TYPE, "STRING_TYPE"},
      // values
      {BOOL_VAL, "BOOL_VAL"}, {INT_VAL, "INT_VAL"},
      {DOUBLE_VAL, "DOUBLE_VAL"}, {STRING_VAL, "STRING_VAL"},
      {CHAR_VAL, "CHAR_VAL"}, {ID, "ID"}, {NIL, "NIL"},
      // eos
      {EOS, "EOS"}
    };
  };

Token::Token()
  : token_type(EOS), token_lexeme(""), token_line(0), token_column(0)
{
}


Token::Token(TokenType type, const std::string& lexeme, int line, int column)
  : token_type(type), token_lexeme(lexeme), token_line(line),
    token_column(column)
{
}


TokenType Token::type() const
{
  return token_type;
}


std::string Token::lexeme() const
{
  return token_lexeme;
}


int Token::line() const
{
  return token_line;
}


int Token::column() const
{
  return token_column;
}


std::string Token::to_string() const
{
  return token_type_map.find(token_type)->second +
    " '" + lexeme() + "' " +
    std::to_string(line()) + ":" + std::to_string(column());
}


#endif
