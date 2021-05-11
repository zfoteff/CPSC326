//----------------------------------------------------------------------
// NAME: S. Bowers
// FILE: mypl_exception.h
// DATE: Spring 2021
// DESC: Exception class for representing MyPL errors during lexical
// analysis, syntax analysis, semantic analysis, and runtime
// execution.
//----------------------------------------------------------------------


#ifndef MYPL_EXCEPTION
#define MYPL_EXCEPTION

// the compilation stage where the error occurred
enum ExceptionType {LEXER, SYNTAX, SEMANTIC, RUNTIME};

// specialized exception for mypl implementation
class MyPLException : public std::exception
{
 public:
  // construct a "normal" error exception
  MyPLException(ExceptionType type, const std::string& msg, int line, int column);

  // return a string representation for printing
  std::string to_string() const;
  
 private:
  ExceptionType type;
  std::string message;
  int line;
  int column;
};


MyPLException::MyPLException(ExceptionType t, const std::string& m, int l, int c)
  : type(t), message(m), line(l), column(c)
{
}


std::string MyPLException::to_string() const
{
  std::string s = "Lexer";
  switch(type) {
    case SYNTAX: s = "Parser"; break;
    case SEMANTIC: s = "Type"; break;
    case RUNTIME: s = "Runtime"; break;
  }
  return s + " Error: " + message + " at line " +
    std::to_string(line) + " column " + std::to_string(column);
}


#endif
