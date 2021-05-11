//----------------------------------------------------------------------
// NAME: S. Bowers
// FILE: ast.h
// DATE: Spring 2021
// DESC: AST types for MyPL implementation. Each AST node is
//       implemented as POD (plain old data) types, with all data
//       public, with the exception of the visitor abstraction (i.e.,
//       the accept function) and a destrutor for dynamic memory. Note
//       that some liberties are taken with formatting to keep the
//       file size manageable.
//----------------------------------------------------------------------

#ifndef AST_H
#define AST_H

#include <list>

//----------------------------------------------------------------------
// Visitor interface
//----------------------------------------------------------------------

// declare to use in Visitor interface
class Program;
class Decl;
class FunDecl;
class TypeDecl;
class Stmt;
class VarDeclStmt;
class AssignStmt;
class ReturnStmt;
class IfStmt;
class WhileStmt;
class ForStmt;
class Expr;
class SimpleTerm;
class ComplexTerm;
class SimpleRValue;
class NewRValue;
class CallExpr;
class IDRValue;
class NegatedRValue;


class Visitor {
public:
  // top-level
  virtual void visit(Program& node) = 0;
  virtual void visit(FunDecl& node) = 0;
  virtual void visit(TypeDecl& node) = 0;
  // statements
  virtual void visit(VarDeclStmt& node) = 0;
  virtual void visit(AssignStmt& node) = 0;
  virtual void visit(ReturnStmt& node) = 0;
  virtual void visit(IfStmt& node) = 0;
  virtual void visit(WhileStmt& node) = 0;
  virtual void visit(ForStmt& node) = 0;
  // expressions
  virtual void visit(Expr& node) = 0;
  virtual void visit(SimpleTerm& node) = 0;
  virtual void visit(ComplexTerm& node) = 0;
  // rvalues (rhs values)
  virtual void visit(SimpleRValue& node) = 0;
  virtual void visit(NewRValue& node) = 0;
  virtual void visit(CallExpr& node) = 0;
  virtual void visit(IDRValue& node) = 0;
  virtual void visit(NegatedRValue& node) = 0;
};


//----------------------------------------------------------------------
// Top-level Abstract AST Nodes
//----------------------------------------------------------------------

// root AST node interface
class ASTNode
{
public:
  virtual ~ASTNode() {};
  virtual void accept(Visitor& v) = 0;
};


// root declaration node
class Decl : public ASTNode
{
};


// root statement node
class Stmt : public ASTNode
{
};


// root expression term
class ExprTerm : public ASTNode
{
public:
  virtual Token first_token() = 0;
};

// root rhs value node
class RValue : public ASTNode
{
public:
  virtual Token first_token() = 0;
};


//----------------------------------------------------------------------
// Expressions and Expression Terms
//----------------------------------------------------------------------


class Expr : public ASTNode
{
public:
  bool negated = false;         // true if not precedes "expression"
  ExprTerm* first = nullptr;    // the first term
  Token* op = nullptr;          // optional operator
  Expr* rest = nullptr;         // expression after operator (if exists)
  // cleanup
  ~Expr() {delete first; delete op; delete rest;}
  // get first token
  Token first_token() {return first->first_token();}
  // visitor access
  void accept(Visitor& v) {v.visit(*this);}
};


class SimpleTerm : public ExprTerm
{
public:
  RValue* rvalue = nullptr;     // one rvalue ("base case")
  // cleanup memory
  ~SimpleTerm() {delete rvalue;}
  // return first token
  Token first_token() {return rvalue->first_token();}  
  // visitor access
  void accept(Visitor& v) {v.visit(*this);}
};  


class ComplexTerm : public ExprTerm
{
public:
  Expr* expr = nullptr;         // term is another expression
  // cleanup memory
  ~ComplexTerm() {delete expr;}
  // return first token
  Token first_token() {return expr->first->first_token();}  
  // visitor access
  void accept(Visitor& v) {v.visit(*this);}
};  


//----------------------------------------------------------------------
// Function, Variable, and Type Declarations
//----------------------------------------------------------------------


class Program : public ASTNode
{
public:
  std::list<Decl*> decls;       //  list of declarations
  // cleanup memory
  ~Program() {for (Decl* d : decls) delete d;}
  // visitor access
  void accept(Visitor& v) {v.visit(*this);}
};


class FunDecl : public Decl
{
public:
  struct FunParam {Token id; Token type;}; // function parameter type
  Token return_type;                       // function return type
  Token id;                                // function name
  std::list<FunParam> params;              // function params
  std::list<Stmt*> stmts;                  // function body 
  // cleanup memory
  ~FunDecl() {for (Stmt* s : stmts) delete s;}
  // visitor access
  void accept(Visitor& v) {v.visit(*this);}
};


class VarDeclStmt : public Stmt
{
public:
  Token* type = nullptr;        // optional variable type
  Token id;                     // variable name
  Expr* expr = nullptr;         // variable initialization expression
  // cleanup memory
  ~VarDeclStmt() {delete type; delete expr;}
  // visitor access
  void accept(Visitor& v) {v.visit(*this);}
};


class TypeDecl : public Decl
{
public:
  Token id;                       // type name
  std::list<VarDeclStmt*> vdecls; // variable declarations
  // cleanup memory
  ~TypeDecl() {for (VarDeclStmt* v : vdecls) delete v;}
  // visitor access
  void accept(Visitor& v) {v.visit(*this);}
};


//----------------------------------------------------------------------
// Statement nodes
//----------------------------------------------------------------------


class AssignStmt : public Stmt
{
public:
  std::list<Token> lvalue_list; // lhs as one or more ids
  Expr* expr = nullptr;         // rhs expression
  // cleanup memory
  ~AssignStmt() {delete expr;}
  // visitor access
  void accept(Visitor& v) {v.visit(*this);}
};


class ReturnStmt : public Stmt
{
public:
  Expr* expr = nullptr;         // return expression
  // cleanup memory
  ~ReturnStmt() {delete expr;}
  // visitor access
  void accept(Visitor& v) {v.visit(*this);}
};  


// helper to store a boolean condition and a body
class BasicIf
{
public:
  Expr* expr = nullptr;   // boolean expression
  std::list<Stmt*> stmts; // body statements
  // cleanup memory
  ~BasicIf() {delete expr; for (Stmt* s : stmts) delete s;}
};


class IfStmt : public Stmt
{
public:
  BasicIf* if_part = nullptr;   // if part
  std::list<BasicIf*> else_ifs; // else ifs
  std::list<Stmt*> body_stmts;  // else body (if empty, no else)
  // cleanup memory
  ~IfStmt()
  {
    delete if_part;
    for (BasicIf* s : else_ifs)
      delete s;
    for (Stmt* s : body_stmts)
      delete s;
  }
  // visitor access
  void accept(Visitor& v) {v.visit(*this);}
};  


class WhileStmt : public Stmt
{
public:
  Expr* expr = nullptr;         // boolean expression
  std::list<Stmt*> stmts;       // body statements
  // cleanup memory
  ~WhileStmt() {delete expr; for (Stmt* s : stmts) delete s;}
  // visitor access
  void accept(Visitor& v) {v.visit(*this);}
};  


class ForStmt : public Stmt
{
public:
  Token var_id;                 // loop variable
  Expr* start;                  // loop start expression
  Expr* end;                    // loop end expression
  std::list<Stmt*> stmts;       // loop body
  // cleanup memory
  ~ForStmt() {delete start; delete end; for (Stmt* s : stmts) delete s;}
  // visitor access
  void accept(Visitor& v) {v.visit(*this);}
};  


//----------------------------------------------------------------------
// RValue nodes
//----------------------------------------------------------------------


class SimpleRValue : public RValue
{
public:
  Token value;                  // primitive value
  // return first token
  Token first_token() {return value;}  
  // visitor access
  void accept(Visitor& v) {v.visit(*this);}
};  


class NewRValue : public RValue
{
public:
  Token type_id;                // type name being instantiated
  // return first token
  Token first_token() {return type_id;}  
  // visitor access
  void accept(Visitor& v) {v.visit(*this);}
};  


class CallExpr : public RValue, public Stmt
{
public:
  Token function_id;            // function name being called
  std::list<Expr*> arg_list;    // call arguments
  // cleanup memory
  ~CallExpr() {for(Expr* e : arg_list) delete e;}
  // return first token
  Token first_token() {return function_id;}  
  // visitor access
  void accept(Visitor& v) {v.visit(*this);}
};  


class IDRValue : public RValue
{
public:
  std::list<Token> path;        // one or more ids (path expression)
  // return first token
  Token first_token() {return path.front();}  
  // visitor access
  void accept(Visitor& v) {v.visit(*this);}
};  


class NegatedRValue : public RValue
{
public:
  Expr* expr = nullptr;         // negated expression
  // cleanup memory
  ~NegatedRValue() {delete expr;}
  // return first token
  Token first_token() {return expr->first->first_token();}  
  // visitor access
  void accept(Visitor& v) {v.visit(*this);}
};  


#endif
