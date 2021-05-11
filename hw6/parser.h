//----------------------------------------------------------------------
// NAME: Zac Foteff
// FILE: parser.h
// DATE: 2/25/2021
// DESC: Implementation of recursive decent parser to build AST
//----------------------------------------------------------------------

#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include "lexer.h"
#include "ast.h"
#include "mypl_exception.h"


class Parser
{
public:

  // create a new recursive descent parser
  Parser(const Lexer& program_lexer);

  // run the parser
  void parse(Program& root_node);

private:
  Lexer lexer;
  Token curr_token;

  // helper functions-
  void advance();
  void eat(TokenType t, std::string err_msg);
  void error(std::string err_msg);
  bool is_operator(TokenType t);
  void dtype();
  void pval(SimpleRValue& node);

  /// --- AST tree building functions --- ///

  //  Declarations
  void tdecl(TypeDecl& node);
  void fdecl(FunDecl& node);

  //  Statements
  void stmt(std::list<Stmt*>& stmts);
  void vdecl_stmt(VarDeclStmt& node);
  void return_stmt(ReturnStmt& node);
  void while_stmt(WhileStmt& node);
  void for_stmt(ForStmt& node);
  void if_stmt(IfStmt& node);

  //  Expressions
  void expr(Expr& node);
  void simple_term(SimpleTerm& node);
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

void Parser::dtype()
{
  if (curr_token.type() == INT_TYPE)
    eat(INT_TYPE, "Expected INT_TYPE ");

  else if (curr_token.type() == DOUBLE_TYPE)
    eat(DOUBLE_TYPE, "Expected DOUBLE_TYPE ");

  else if (curr_token.type() == BOOL_TYPE)
    eat(BOOL_TYPE, "Expected BOOL_TYPE ");

  else if (curr_token.type() == CHAR_TYPE)
    eat(CHAR_TYPE, "Expected CHAR_TYPE ");

  else if (curr_token.type() == STRING_TYPE)
    eat(STRING_TYPE, "Expected STRING_TYPE ");

  else if (curr_token.type() == NIL)
    eat(NIL, "Expected NIL ");

  else
    error("Expected type declaration ");
}

// Recursive-decent functions"
void Parser::parse(Program& root_node)
{
  advance();
  while (curr_token.type() != EOS)
  {
    if (curr_token.type() == TYPE)
    {
      //  Declare an visit new Type declaration
      TypeDecl* t = new TypeDecl;
      tdecl(*t);
      root_node.decls.push_back(t);
    }

    else if (curr_token.type() == FUN)
    {
      FunDecl* f = new FunDecl();
      fdecl(*f);
      root_node.decls.push_back(f);
    }

    else
      error("Expected FUN or TYPE declaration ");
  }
  eat(EOS, "Expecting end-of-file ");
}

//  ----------------------------
//        Declarations
//  ----------------------------
void Parser::tdecl(TypeDecl& node)
{
  //  Type Declaration case
  eat(TYPE, "Expected TYPE");
  node.id = curr_token;
  eat(ID, "Expected ID");

  //  Read all member declarations until end
  while (curr_token.type() != END)
  {
    VarDeclStmt* v = new VarDeclStmt();
    vdecl_stmt(*v);
    node.vdecls.push_back(v);
  }

  eat(END, "Expected END");
}

void Parser::fdecl(FunDecl& node)
{
  eat(FUN, "Expected FUN");
  node.return_type = curr_token;
  advance();
  node.id = curr_token;
  eat(ID, "Expected ID ");
  eat(LPAREN, "Expected LPAREN");

  //  Read all params in the function id
  while (curr_token.type() != RPAREN)
  {
    FunDecl::FunParam f;
    f.id = curr_token;
    advance();
    eat(COLON, "Expected COLON ");
    f.type = curr_token;
    advance();
    node.params.push_back(f);

    //  If there is more than one param, eat the comma and repeat process
    if (curr_token.type() == COMMA)
      eat(COMMA, "Expected COMMA ");
  }

  //  Eat closing paren for the param list
  eat(RPAREN, "Expected RPAREN");

  //  Add all statements in function body to its stmt_list
  while (curr_token.type() != END)
    stmt(node.stmts);

  eat(END, "Expected END");
}

// -----------------------------
//        Statements
// -----------------------------
void Parser::stmt(std::list<Stmt*>& stmts)
{
  //  Examine curr token to determine which of the 7 statements should be called
  switch(curr_token.type())
  {
    case VAR:
    {
      //  VarDecl case
      VarDeclStmt* v = new VarDeclStmt();
      vdecl_stmt(*v);
      stmts.push_back(v);
      break;
    }

    case FOR:
    {
      //  ForStmt case
      ForStmt* f = new ForStmt();
      for_stmt(*f);
      stmts.push_back(f);
      break;
    }

    case WHILE:
    {
      //  WhileStmt case
      WhileStmt* w = new WhileStmt();
      while_stmt(*w);
      stmts.push_back(w);
      break;
    }

    case RETURN:
    {
      //  Return stmt case
      ReturnStmt* r = new ReturnStmt();
      return_stmt(*r);
      stmts.push_back(r);
      break;
    }

    case IF:
    {
      //  IfStmt case
      IfStmt* i = new IfStmt();
      if_stmt(*i);
      stmts.push_back(i);
      break;
    }

    case ID:
    {
      //  Need to check if the statement should be a CallExpr or an AssignStmt
      Token new_id = curr_token;
      eat(ID, "Expected ID");

      if (curr_token.type() == LPAREN)
      {
        //  Call expression case
        eat(LPAREN, "Expected LPAREN ");
        CallExpr* c = new CallExpr();
        c->function_id = new_id;

        // Build in check for idrval

        while (curr_token.type() != RPAREN)
        {
          //  While the statement isn't close keep adding new expressions
          Expr* e = new Expr();
          expr(*e);
          c->arg_list.push_back(e);
        }

        //  Eat closing param
        eat(RPAREN, "Expected RPAREN ");
        stmts.push_back(c);
      }

      else
      {
        //  Variable assignment case
        AssignStmt* a = new AssignStmt();
        a->lvalue_list.push_back(new_id);

        //  Add id path to the assign stmt
        while (curr_token.type() != ASSIGN)
        {
          eat(DOT, "Expected DOT ");
          a->lvalue_list.push_back(curr_token);
          eat(ID, "Expected ID ");
        }

        eat(ASSIGN, "Expected ASSIGN ");
        Expr* e = new Expr();
        expr(*e);
        a->expr = e;
        stmts.push_back(a);
      }

      break;
    }

    default:
      error("Unexpected token ");
      break;
  }
}

void Parser::vdecl_stmt(VarDeclStmt& node)
{
  eat(VAR, "Expected VAR ");
  node.id = curr_token;
  eat(ID, "Expected ID ");

  //  Check to see if the new variable is directly typed
  if (curr_token.type() == COLON)
  {
    eat(COLON, "Expected COLON ");
    Token* new_id = new Token();
    *new_id = curr_token;
    node.type = new_id;
    advance();
  }

  eat(ASSIGN, "Expected ASSIGN ");
  //  Call the expression for rhs of assignment
  Expr* e = new Expr();
  expr(*e);
  node.expr = e;
}

void Parser::while_stmt(WhileStmt& node)
{
  eat(WHILE, "Expected WHILE ");
  Expr* e = new Expr();
  expr(*e);
  node.expr = e;
  eat(DO, "Expected DO ");

  //  Initialize new list for all possible stmts in the while loop
  std::list<Stmt*> stmt_list;

  // Until the end of the loop, add stmts
  while (curr_token.type() != END)
    stmt(stmt_list);

  eat(END, "Expected END ");
  node.stmts = stmt_list;
}

void Parser::for_stmt(ForStmt& node)
{
  eat(FOR, "Expected FOR ");
  node.var_id = curr_token;
  Expr* start = new Expr();
  Expr* end = new Expr();
  expr(*start);
  eat(TO, "Expected TO ");
  expr(*end);
  node.start = start;
  node.end = end;
  eat(DO, "Expected DO ");

  //  Initialize new list for all possible stmts in the for loop
  std::list<Stmt*> stmt_list;

  // Until the end of the loop, add stmts
  while (curr_token.type() != END)
    stmt(stmt_list);

  eat(END, "Expected END ");
  node.stmts = stmt_list;
}

void Parser::if_stmt(IfStmt& node)
{
  BasicIf* bi = new BasicIf();
  //  initialize list for Basic If statements
  std::list<Stmt*> bi_stmt_list;
  //  initialize list for elseif statments
  std::list<BasicIf*> else_if_stmt_list;
  //  initialize list for else stmts
  std::list<Stmt*> else_stmt_list;

  eat(IF, "Expected IF ");
  Expr* e = new Expr();
  expr(*e);
  bi->expr = e;
  eat(THEN, "Expected THEN");

  while (curr_token.type() != END && curr_token.type() != ELSEIF && curr_token.type() != ELSE)
    stmt(bi->stmts);

  if (curr_token.type() == ELSEIF)
  {
    //  Read through all ELSEIF statements
    while (curr_token.type() == ELSEIF)
    {
      //  initialize new Basic if statement for else if cases
      BasicIf* new_bi = new BasicIf;
      //  initialize stmt list for new Basic If statements
      std::list<Stmt*> new_bi_stmt_list;
      eat(ELSEIF, "Expected ELSEIF");
      Expr* e = new Expr();
      expr(*e);
      new_bi->expr = e;
      eat(THEN, "Expected Then");

      while (curr_token.type() != END && curr_token.type() != ELSE && curr_token.type() != ELSEIF)
        stmt(new_bi_stmt_list);

      new_bi->stmts = new_bi_stmt_list;
      else_if_stmt_list.push_back(new_bi);
    }
  }

  if (curr_token.type() == ELSE)
  {
    eat(ELSE, "Expected ELSE");

    while (curr_token.type() != END)
      stmt(else_stmt_list);
  }

  eat(END, "Expected END ");

  //  Connect statments to if statement
  node.if_part = bi;
  node.else_ifs = else_if_stmt_list;
  node.body_stmts = else_stmt_list;
}

void Parser::return_stmt(ReturnStmt& node)
{
  eat(RETURN, "Expected RETURN ");
  Expr* e = new Expr();
  expr(*e);
  node.expr = e;
}

//  --------------------------------
//            Expressions
//  --------------------------------
void Parser::expr(Expr& node)
{
  if (curr_token.type() == NEG)
  {
    eat(NEG, "Expected NEG ");
    node.negated = true;
  }

  if (curr_token.type() == NOT)
  {
    //  We know this has to be a ComplexStmt
    ComplexTerm* c = new ComplexTerm();
    eat(NOT, "Expected NOT ");
    node.negated = true;
    Expr* e = new Expr();
    expr(*e);
    c->expr = e;
    node.first = c;
  }

  else if (curr_token.type() == LPAREN)
  {
    //  LPAREN implies a complex stmt
    eat(LPAREN, "Expected LPAREN ");
    ComplexTerm* c = new ComplexTerm();
    Expr* e = new Expr();
    expr(*e);
    c->expr = e;
    node.first = c;

    //  Check for closing paran
    if (curr_token.type() == RPAREN)
      eat(RPAREN, "Expected RPAREN");
  }

  else
  {
    //  Create a simple statement
    SimpleTerm* s = new SimpleTerm();
    simple_term(*s);
    node.first = s;
  }

  if (is_operator(curr_token.type()) || curr_token.type() == ASSIGN)
  {
    //  Copy token into op
    Token* new_op = new Token;
    *new_op = curr_token;
    node.op = new_op;
    advance();
    Expr* e = new Expr();
    expr(*e);
    node.rest = e;

    //  Eat closing paren
    if (curr_token.type() == RPAREN)
      eat(RPAREN, "Expected RPAREN ");
  }
}

void Parser::simple_term(SimpleTerm& node)
{
  if (curr_token.type() == INT_VAL || curr_token.type() == DOUBLE_VAL
      || curr_token.type() == BOOL_VAL || curr_token.type() == CHAR_VAL
      || curr_token.type() == STRING_VAL || curr_token.type() == NIL)
  {
    //  Simple RValue case
    SimpleRValue* s = new SimpleRValue();
    pval(*s);
    node.rvalue = s;
  }

  else if (curr_token.type() == NEW)
  {
    //  NewRValue Case
    NewRValue* n = new NewRValue();
    eat(NEW, "Expected NEW ");
    n->type_id = curr_token;
    eat(ID, "Expected Type ID ");
    node.rvalue = n;
  }

  else if (curr_token.type() == NEG)
  {
    //  NegatedRValue Case
    NegatedRValue* n = new NegatedRValue();
    expr(*n->expr);
    node.rvalue = n;
  }

  else if (curr_token.type() == ID)
  {
    //  Determine if this should create an IDR val, AssignStmt, or CallExpr
    Token new_id = curr_token;
    eat(ID, "Expected ID ");

    //  CallExpr
    if (curr_token.type() == LPAREN)
    {
      //  CallExpr case
      eat(LPAREN, "Expected LPAREN ");
      CallExpr* c = new CallExpr;
      c->function_id = new_id;

      while (curr_token.type() != RPAREN)
      {
        Expr* e = new Expr();
        expr(*e);
        c->arg_list.push_back(e);

        //  If there is more than one param, eat the comma and repeat loop
        if (curr_token.type() == COMMA)
          eat(COMMA, "Expected COMMA ");
      }

      eat(RPAREN, "Expected RPAREN ");
      node.rvalue = c;
    }

    else
    {
      //  IDRValue case
      IDRValue* v = new IDRValue();
      v->path.push_back(new_id);

      while(curr_token.type() == DOT)
      {
        eat(DOT, "Expected DOT ");
        v->path.push_back(curr_token);
        eat(ID, "Expected ID ");
      }

      node.rvalue = v;
    }
  }

  else
    error("Something went wrong ");
}

void Parser::pval(SimpleRValue& node)
{
  node.value = curr_token;
  switch(curr_token.type())
  {
    case INT_VAL:
      eat(INT_VAL, "Expected INT_VAL ");
      break;

    case CHAR_VAL:
      eat(CHAR_VAL, "Expected CHAR_VAL");
      break;

    case BOOL_VAL:
      eat(BOOL_VAL, "Expected BOOL_VAL ");
      break;

    case DOUBLE_VAL:
      eat(DOUBLE_VAL, "Expected INT_VAL ");
      break;

    case STRING_VAL:
      eat(STRING_VAL, "Expected STRING_VAL ");
      break;

    case NIL:
      eat(NIL, "Expected NIL ");
      break;

    default:
      error("Unexpected Value");
      break;
  }
}

#endif
