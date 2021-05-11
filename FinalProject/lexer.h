//----------------------------------------------------------------------
// NAME:  Zac Foteff
// FILE:  lexer.h
// DATE:  2/4/2021
// DESC:  Definition and implementation of a Lexer class of MyPL
//----------------------------------------------------------------------

#ifndef LEXER_H
#define LEXER_H

#include <istream>
#include <string>
#include "token.h"
#include "mypl_exception.h"
using namespace std;

class Lexer
{
public:

  // construct a new lexer from the input stream
  Lexer(std::istream& input_stream);

  // return the next available token in the input stream (including
  // EOS if at the end of the stream)
  Token next_token();

private:

  // input stream, current line, and current columnumn
  std::istream& input_stream;
  int line = 1;
  int column = 0;

  // return a single character from the input stream and advance
  char read();

  // return a single character from the input stream without advancing
  char peek();

  // create and throw a mypl_exception (exits the lexer)
  void error(const std::string& msg, int line, int column) const;
};


Lexer::Lexer(std::istream& input_stream)
  : input_stream(input_stream), line(1), column(1)
{
}

char Lexer::read()
{
  return input_stream.get();
}

char Lexer::peek()
{
  return input_stream.peek();
}

void Lexer::error(const std::string& msg, int line, int column) const
{
  throw MyPLException(LEXER, msg, line, column);
}

Token Lexer::next_token()
{
  //  Initialize the empty lexeme string
  string lexeme = "";
  char ch = read();
  column++;

  //  1. Check for whitespace and comments and read through it
  while (isspace(ch))
  {
    //  Whitespace
    if (ch == ' ')
    {
      ch = read();
      column++;
    }

    //  newline
    if (ch == '\n')
    {
      ch = read();
      line++;
      column = 1;
    }

    if (ch == '\t')
    {
      ch = read();
      column++;
    }
  }

  //  2. Check for comments
  if (ch == '#')
  {
    bool multiline = true;
    //  Read until newline to read out all of the comment

    while (multiline)
    {
      while (ch != '\n')
        ch = read();

      //  incriment the line number and reset the column count
      line++;
      column = 1;
      ch = read();

      while (isspace(ch))
      {
        //  Whitespace
        if (ch == ' ')
        {
          ch = read();
          column++;
        }

        //  newline
        if (ch == '\n')
        {
          ch = read();
          line++;
          column = 1;
        }
      }

      if (ch != '#')
        multiline = false;
    }
  }

  //  3. Check for EOF
  if (ch == EOF)
    return Token(EOS, "", line, column);

  //  4. Check for simple symbols
  else if (ch == ',')
  {
    lexeme += ch;
    return Token(COMMA, lexeme, line, column);
  }

  else if (ch == '.')
  {
    lexeme += ch;
    return Token(DOT, lexeme, line, column);
  }

  else if (ch == ':')
  {
    lexeme += ch;
    return Token(COLON, lexeme, line, column);
  }

  else if (ch == '+')
  {
    lexeme += ch;
    return Token(PLUS, lexeme, line, column);
  }

  else if (ch == '-')
  {
    lexeme += ch;
    return Token(MINUS, lexeme, line, column);
  }

  else if (ch == '*')
  {
    lexeme += ch;
    return Token(MULTIPLY, lexeme, line, column);
  }

  else if (ch == '/')
  {
    lexeme += ch;
    return Token(DIVIDE, lexeme, line, column);
  }

  else if (ch == '%')
  {
    lexeme += ch;
    return Token(MODULO, lexeme, line, column);
  }

  else if (ch == '(')
  {
    lexeme += ch;
    return Token(LPAREN, lexeme, line, column);
  }

  else if (ch == ')')
  {
    lexeme += ch;
    return Token(RPAREN, lexeme, line, column);
  }

  //  5. More complex symbols
  else if (ch == '=')
  {
    lexeme += ch;
    if (peek() == '=')
    {
      //  If next char == '=' that means the expected token is EQUAL
      int start_col = column;
      lexeme += read();
      column++;
      return Token(EQUAL, lexeme, line, start_col);
    }

    return Token(ASSIGN, lexeme, line, column);
  }

  else if (ch == '<')
  {
    lexeme += ch;
    if (peek() == '=')
    {
      //  If next char == '=' that means expected token is LESS_EQUAL
      int start_col = column;
      lexeme += read();
      column++;
      return Token(LESS_EQUAL, lexeme, line, start_col);
    }

    return Token(LESS, lexeme, line, column);
  }

  else if (ch == '>')
  {
    lexeme += ch;
    if (peek() == '=')
    {
      //  If next char == '=' that means expected toke is GREATER_EQUAL
      int start_col = column;
      lexeme += read();
      column++;
      return Token(GREATER_EQUAL, lexeme, line, start_col);
    }

    return Token(GREATER, lexeme, line, column);
  }

  else if (ch == '!')
  {
    lexeme += ch;
    if (peek() == '!')
      error("Illegal character: !!", line, column);

    if (peek() == '=')
    {
      //  If next char == '=' that means expected toke is NOT_EQUAL
      int start_col = column;
      lexeme += read();
      column++;
      return Token(NOT_EQUAL, lexeme, line, start_col);
    }

    return Token(NEG, lexeme, line, column);
  }

  //  6. Check for character values
  else if (ch == '\'')
  {
    int start_col = column;

    //  If next character is closing apostrophe, report error
    if (peek() == '\'')
      error("Illegal character: ''", line, start_col);

    ch = read();
    column++;

    //  Error: user has declared a character and assigned it to a non-char value
    if (!(isalpha(ch)))
      error("char must be an alphabetical character", line, start_col);

    //  Error: user has entered more than one char into the statement
    if (peek() != '\'')
      error("char can only contain one character", line, start_col);

    lexeme += ch;
    ch = read();
    column++;

    return Token(CHAR_VAL, lexeme, line, start_col);
  }

  //  7. Check for string values
  else if (ch == '"')
  {
    int start_col = column;
    ch = read();
    column++;

    if (ch == '"')
      return Token(STRING_VAL, "\"\"", line, start_col);

    while (peek() != '"')
    {
      //  if newline appears in the string throw an error
      if (isspace(ch))
      {
        //  Allows for \n characters to appear in strings while also still allowing for error
        if (ch == '\n' && isspace(peek()))
          error("Strings need to be one continuous string of characters", line, start_col);
      }

      lexeme += ch;
      ch = read();
      column++;
    }

    lexeme += ch;
    ch = read();

    return Token(STRING_VAL, lexeme, line, start_col);
  }

  //  8. Check for numbers
  else if (isdigit(ch))
  {
    int start_col = column;
    bool is_double = false;
    lexeme += ch;

    while (isdigit(peek()) || peek() == '.')
    {
      //  If the char is a dot, flag the lexeme as a double
      if (ch == '.')
        is_double = true;

      lexeme += ch;
      ch = read();
      column++;
    }

    if (isalpha(peek()))
      error ("Int followed by id without space", line, start_col);

    if (is_double)
      return Token(DOUBLE_VAL, lexeme, line, start_col);

    return Token(INT_VAL, lexeme, line, start_col);
  }

  //  9. Check for reserved words/IDs
  else
  {
    bool is_id = false;
    int start_col = column;
    lexeme += ch;

    while (!(isspace(peek())))
    {
      //  build lexeme, trigger flag if non-alphabetical character is found
      if (isdigit(peek()) || peek() == '_')
        is_id = true;

      //  Stop reading if the character isn't a legal character
      if (((!(isalpha(peek())) && !(isdigit(peek()))) && !is_id)
      || peek() == ':' || peek() == '(' || peek() == ')'
      || peek() == ',' || peek() == '=')
        break;

      ch = read();
      lexeme += ch;
      column++;
    }

    if (is_id)
      return Token(ID, lexeme, line, start_col);

    if (lexeme == "and")
      return Token(AND, lexeme, line, start_col);

    if (lexeme == "or")
      return Token(OR, lexeme, line, start_col);

    if (lexeme == "not")
      return Token(NOT, lexeme, line, start_col);

    if (lexeme == "type")
      return Token(TYPE, lexeme, line, start_col);

    if (lexeme == "while")
      return Token(WHILE, lexeme, line, start_col);

    if (lexeme == "for")
      return Token(FOR, lexeme, line, start_col);

    if (lexeme == "to")
      return Token(TO, lexeme, line, start_col);

    if (lexeme == "do")
      return Token(DO, lexeme, line, start_col);

    if (lexeme == "if")
      return Token(IF, lexeme, line, start_col);

    if (lexeme == "then")
      return Token(THEN, lexeme, line, start_col);

    if (lexeme == "elseif")
      return Token(ELSEIF, lexeme, line, start_col);

    if (lexeme == "else")
      return Token(ELSE, lexeme, line, start_col);

    if (lexeme == "end")
      return Token(END, lexeme, line, start_col);

    if (lexeme == "fun")
      return Token(FUN, lexeme, line, start_col);

    if (lexeme == "var")
      return Token(VAR, lexeme, line, start_col);

    if (lexeme == "return")
      return Token(RETURN, lexeme, line, start_col);

    if (lexeme == "new")
      return Token(NEW, lexeme, line, start_col);

    if (lexeme == "try")
      return Token(TRY, lexeme, line, start_col);

    if (lexeme == "catch")
      return Token(CATCH, lexeme, line, start_col);

    if (lexeme == "throw")
      return Token(THROW, lexeme, line, start_col);

    if (lexeme == "bool")
      return Token(BOOL_TYPE, lexeme, line, start_col);

    if (lexeme == "int")
      return Token(INT_TYPE, lexeme, line, start_col);

    if (lexeme == "double")
      return Token(DOUBLE_TYPE, lexeme, line, start_col);

    if (lexeme == "char")
      return Token(CHAR_TYPE, lexeme, line, start_col);

    if (lexeme == "string")
      return Token(STRING_TYPE, lexeme, line, start_col);

    if (lexeme == "true")
      return Token(BOOL_VAL, lexeme, line, start_col);

    if (lexeme == "false")
      return Token(BOOL_VAL, lexeme, line, start_col);

    if (lexeme == "neg")
      return Token(NEG, lexeme, line, start_col);

    if (lexeme == "nil")
      return Token(NIL, lexeme, line, start_col);

    else
      return Token(ID, lexeme, line, start_col);
  }

  return Token(EOS, "", line, column);
}

#endif
