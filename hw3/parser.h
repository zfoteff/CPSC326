//----------------------------------------------------------------------
// NAME: Zac Foteff
// FILE: parser.h
// DATE: 2/16/2021
// DESC: Implementation of recursive decent parser
//----------------------------------------------------------------------

#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include "lexer.h"
#include "mypl_exception.h"


class Parser
{
public:

  // create a new recursive descent parser
  Parser(const Lexer& program_lexer);

  // run the parser
  void parse();

private:
  Lexer lexer;
  Token curr_token;

  // helper functions
  void advance();
  void eat(TokenType t, std::string err_msg);
  void error(std::string err_msg);
  bool is_operator(TokenType t);

  // recursive descent functions
  void program();
  void tdecl();
  void vdecls();
  void fdecl();
  void params();
  void dtype();
  void stmts();
  void stmt();
  void vdecl_stmt();
  void assign_stmt();
  void lvalue();
  void rvalue();
  void cond_stmt();
  void condt();
  void while_stmt();
  void for_stmt();
  void args();
  void exit_stmt();
  void expr();
  void op();
  void pval();
};


// constructor
Parser::Parser(const Lexer& program_lexer) : lexer(program_lexer)
{
}

// Helper functions

void Parser::advance()
{
  curr_token = lexer.next_token();
}

void Parser::eat(TokenType t, std::string err_msg)
{
  if (curr_token.type() == t)
    advance();
  else
    error(err_msg);
}

void Parser::error(std::string err_msg)
{
  std::string s = err_msg + "found '" + curr_token.lexeme() + "'";
  int line = curr_token.line();
  int col = curr_token.column();
  throw MyPLException(SYNTAX, s, line, col);
}

bool Parser::is_operator(TokenType t)
{
  return t == PLUS or t == MINUS or t == DIVIDE or t == MULTIPLY or
    t == MODULO or t == AND or t == OR or t == EQUAL or t == LESS or
    t == GREATER or t == LESS_EQUAL or t == GREATER_EQUAL or t == NOT_EQUAL;
}

// Recursive-decent functions
void Parser::parse()
{
  advance();
  while (curr_token.type() != EOS) {
    if (curr_token.type() == TYPE)
      tdecl();
    else
      fdecl();
  }
  eat(EOS, "expecting end-of-file ");
}

void Parser::tdecl()
{
  eat(TYPE, "Expected TYPE");
  eat(ID, "Expected ID");
  vdecls();
  eat(END, "Expected END");
}

void Parser::vdecls()
{
  if (curr_token.type() == VAR)
  {
    vdecl_stmt();
    vdecls();
  }
}

void Parser::vdecl_stmt()
{
  eat(VAR, "Expected VAR");
  eat(ID, "Expected ID");

  if (curr_token.type() == COLON)
  {
    eat(COLON, "Expected COLON");
    dtype();
  }

  eat(ASSIGN, "Expected ASSIGN ");
  expr();
}

void Parser::fdecl()
{
  // TODO
  eat(FUN, "Expected FUN");

  if (curr_token.type() == INT_TYPE)
    eat(INT_TYPE, "Expected int type");

  else if (curr_token.type() == DOUBLE_TYPE)
    eat(DOUBLE_TYPE, "Expected double type");

  else if (curr_token.type() == BOOL_TYPE)
    eat(BOOL_TYPE, "Expected bool type");

  else if (curr_token.type() == CHAR_TYPE)
    eat(CHAR_TYPE, "Expected char type");

  else if (curr_token.type() == STRING_TYPE)
    eat(DOUBLE_TYPE, "Expected string type");

  else if (curr_token.type() == ID)
    eat(ID, "Expected id");

  else if (curr_token.type() == NIL)
    eat(NIL, "Expected NIL");

  else
    error("Encountered unexpected token");

  eat(ID, "Expected ID");
  eat(LPAREN, "Expected LPAREN ");
  params();
  eat(RPAREN, "Expected RPAREN");
  stmts();
  eat(END, "Expected END");
}

void Parser::params()
{
  if (curr_token.type() == ID)
  {
    eat(ID, "Expected ID");
    eat(COLON, "Expected COLON");
    dtype();
    while (curr_token.type() == COMMA)
    {
      eat(COMMA, "Expected COMMA");
      eat(ID, "Expected ID");
      eat(COLON, "Expected COLON");
      dtype();
    }
  }
}

void Parser::dtype()
{
  if (curr_token.type() == INT_TYPE)
    eat(INT_TYPE, "Expected int type");

  else if (curr_token.type() == DOUBLE_TYPE)
    eat(DOUBLE_TYPE, "Expected double type");

  else if (curr_token.type() == BOOL_TYPE)
    eat(BOOL_TYPE, "Expected bool type");

  else if (curr_token.type() == CHAR_TYPE)
    eat(CHAR_TYPE, "Expected char type");

  else if (curr_token.type() == STRING_TYPE)
    eat(STRING_TYPE, "Expected string type");

  else if (curr_token.type() == ID)
    eat(ID, "Expected id");

  else
    error("Encountered unexpected token");
}

void Parser::stmts()
{
  if (curr_token.type() == VAR || curr_token.type() == ASSIGN
    || curr_token.type() == ID || curr_token.type() == IF
    || curr_token.type() == WHILE || curr_token.type() == FOR
    || curr_token.type() == RETURN)
  {
    stmt();
    stmts();
  }
}

void Parser::stmt()
{
  if (curr_token.type() == VAR)
    vdecl_stmt();

  else if (curr_token.type() == ASSIGN)
    assign_stmt();

  else if (curr_token.type() == ID)
  {
    //  Could be assign_stmt (lvalue), or call_expr()
    eat(ID,"Expected ID ");
    if (curr_token.type() == LPAREN)
    {
      //  call_expr case
      eat(LPAREN, "Expected LPAREN ");
      args();
      eat(RPAREN, "Expected RPAREN ");
    }

    else if (curr_token.type() == DOT || curr_token.type() == ASSIGN)
    {
      //  assign_stmt case
      while (curr_token.type() == DOT)
      {
        //  Handle LVALUE
        eat(DOT, "Expected DOT ");
        eat(ID, "Expected ID ");
      }

      eat(ASSIGN, "Expected ASSIGN ");
      expr();
    }
  }

  else if (curr_token.type() == IF)
    cond_stmt();

  else if (curr_token.type() == WHILE)
    while_stmt();

  else if (curr_token.type() == FOR)
    for_stmt();

  else if (curr_token.type() == RETURN)
    exit_stmt();

  else
    error("Encountered unexpected token");
}

void Parser::assign_stmt()
{
  lvalue();
  eat(ASSIGN, "Expected ASSIGN ");
  expr();
}

void Parser::cond_stmt()
{
  eat(IF, "Expected IF");
  expr();
  eat(THEN, "Expected THEN");
  stmts();
  condt();
  eat(END, "Expected END");
}

void Parser::condt()
{
  if (curr_token.type() == ELSEIF)
  {
    eat(ELSEIF, "Expected ELSEIF");
    expr();
    eat(THEN, "Expected THEN");
    stmts();
    condt();
  }

  if (curr_token.type() == ELSE)
  {
    eat(ELSE, "Expected ELSE");
    stmts();
  }
}

void Parser::for_stmt()
{
  eat(FOR, "Expected FOR");
  eat(ID, "Expected ID");
  eat(ASSIGN, "Expected ASSIGN ");
  expr();
  eat(TO, "Expected TO");
  expr();
  eat(DO, "Expected DO");
  stmts();
  eat(END, "Expected END");
}

void Parser::while_stmt()
{
  eat(WHILE, "Expected WHILE");
  expr();
  eat(DO, "Expected DO");
  stmts();
  eat(END, "Expected END");
}

void Parser::exit_stmt()
{
  eat(RETURN, "Expected RETURN");
  expr();
}

void Parser::expr()
{
  if (curr_token.type() == NOT)
  {
    eat(NOT, "Expected NOT");
    expr();
  }

  else if (curr_token.type() == LPAREN)
  {
    eat(LPAREN, "Expected LPAREN ");
    expr();
    eat(RPAREN, "Expected RPAREN ");
  }

  else
    rvalue();

  if (is_operator(curr_token.type()))
  {
    op();
    expr();
  }
}

void Parser::args()
{
  expr();
  while (curr_token.type() == COMMA)
  {
    eat(COMMA, "Expected COMMA");
    expr();
  }
}

void Parser::op()
{
  if (curr_token.type() == PLUS)
    eat(PLUS, "Expected PLUS");

  else if (curr_token.type() == MINUS)
    eat(MINUS, "Expected MINUS");

  else if (curr_token.type() == DIVIDE)
    eat(DIVIDE, "Expected DIVIDE");

  else if (curr_token.type() == MULTIPLY)
    eat(MULTIPLY, "Expected MULTIPLY ");

  else if (curr_token.type() == MODULO)
    eat(MODULO, "Expected MODULO" );

  else if (curr_token.type() == AND)
    eat(AND, "Expected AND ");

  else if (curr_token.type() == OR)
    eat(OR, "Expected OR ");

  else if (curr_token.type() == EQUAL)
    eat(EQUAL, "Expected EQUAL" );

  else if (curr_token.type() == LESS)
    eat(LESS, "Expected LESS ");

  else if (curr_token.type() == GREATER)
    eat(GREATER, "Expected GREATER ");

  else if (curr_token.type() == LESS_EQUAL)
    eat(LESS_EQUAL, "Expected LESS_EQUAL ");

  else if (curr_token.type() == GREATER_EQUAL)
    eat(GREATER_EQUAL, "Expected GREATER_EQUAL ");

  else if (curr_token.type() == NOT_EQUAL)
    eat(NOT_EQUAL, "Expected NOT EQUAL ");

  else
    error("Encountered unexpected token");
}

void Parser::lvalue()
{
  eat (ID, "Expected ID ");

  while(curr_token.type() == DOT)
  {
    eat(DOT, "Expected DOT ");
    eat(ID, "Expected ID ");
  }
}

void Parser::pval()
{
  if (curr_token.type() == INT_VAL)
    eat(INT_VAL, "Expected INT_VAL");

  else if (curr_token.type() == DOUBLE_VAL)
    eat(DOUBLE_VAL, "Expected DOUBLE_VAL");

  else if (curr_token.type() == BOOL_VAL)
    eat(BOOL_VAL, "Expected BOOL_VAL");

  else if (curr_token.type() == CHAR_VAL)
    eat(CHAR_VAL, "Expected CHAR_VAL");

  else if (curr_token.type() == STRING_VAL)
    eat(STRING_VAL, "Expected STRING_VAL");

  else
    error("Encountered unexpected token");
}

void Parser::rvalue()
{
  if (curr_token.type() == NIL)
    eat(NIL, "Expected NIL ");

  else if (curr_token.type() == NEW)
  {
    eat(NEW, "Expected NEW ");
    eat(ID, "Expected ID ");
  }

  else if (curr_token.type() == NEG)
  {
    eat(NEG, "Expected NEG ");
    expr();
  }

  else if (curr_token.type() == ID)
  {
    eat(ID, "Expected ID ");

    //  <call_expr>
    if (curr_token.type() == LPAREN)
    {
      eat(LPAREN, "Expected LPAREN ");
      args();
      eat(RPAREN, "Expected RPAREN ");
    }

    //  <idrval>
    else
    {
      while (curr_token.type() == DOT)
      {
        eat(DOT, "Expected DOT ");
        eat(ID, "Expected ID");
      }
    }
  }

  else if (curr_token.type() == INT_VAL || curr_token.type() == DOUBLE_VAL
          || curr_token.type() == BOOL_VAL || curr_token.type() == CHAR_VAL
          || curr_token.type() == STRING_VAL)
  {
    pval();
  }
}

#endif
