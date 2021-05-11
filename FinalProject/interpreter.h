
//----------------------------------------------------------------------
// NAME: Zac Foteff
// FILE: interpreter.h
// DATE: 4/6/21
// DESC: Interpreter for MyPL that executes MyPL code
//----------------------------------------------------------------------
#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <iostream>
#include <unordered_map>
#include <regex>
#include "ast.h"
#include "symbol_table.h"
#include "data_object.h"
#include "heap.h"
#include "mypl_exception.h"



class Interpreter : public Visitor
{
public:

  // top-level
  void visit(Program& node);
  void visit(FunDecl& node);
  void visit(TypeDecl& node);
  // statements
  void visit(VarDeclStmt& node);
  void visit(AssignStmt& node);
  void visit(ReturnStmt& node);
  void visit(IfStmt& node);
  void visit(WhileStmt& node);
  void visit(ForStmt& node);

  //  Final project declarations
  void visit(TryStmt& node);
  void visit(CatchStmt& node);
  void visit(ThrowStmt& node);

  // expressions
  void visit(Expr& node);
  void visit(SimpleTerm& node);
  void visit(ComplexTerm& node);
  // rvalues
  void visit(SimpleRValue& node);
  void visit(NewRValue& node);
  void visit(CallExpr& node);
  void visit(IDRValue& node);
  void visit(NegatedRValue& node);

  // return code from calling main
  int return_code() const;


private:

  // return exception
  class MyPLReturnException : public std::exception {};

  // the symbol table
  SymbolTable sym_table;

  // holds the previously computed value
  DataObject curr_val;

  // the heap
  Heap heap;

  // the next oid
  size_t next_oid = 0;

  // the functions (all within the global environment)
  std::unordered_map<std::string,FunDecl*> functions;

  // the user-defined types (all within the global environment)
  std::unordered_map<std::string,TypeDecl*> types;

  // the global environment id
  int global_env_id = 0;

  // the program return code
  int ret_code = 0;

  // error message
  void error(const std::string& msg, const Token& token);
  void zerodiv_error(const std::string& msg);
  void index_error(const std::string& msg);
  void error(const std::string& msg);
};


int Interpreter::return_code() const
{
  return ret_code;
}

void Interpreter::error(const std::string& msg, const Token& token)
{
  throw MyPLException(RUNTIME, msg, token.line(), token.column());
}

void Interpreter::error(const std::string& msg)
{
  throw MyPLException(RUNTIME, msg);
}

//  Final project error methods
void Interpreter::zerodiv_error(const std::string& msg)
{
  throw MyPLException(ZERODIVISION, msg);
}

void Interpreter::index_error(const std::string& msg)
{
  throw MyPLException(INDEXOUTOFBOUNDS, msg);
}

// top-level
void Interpreter::visit(Program& node)
{
  //  Push global environment
  sym_table.push_environment();
  global_env_id = sym_table.get_environment_id();

  //  Iterate through declarations in the Prog node
  for (Decl* d : node.decls)
    d->accept(*this);

  //  Call main function
  CallExpr expr;
  expr.function_id = functions["main"]->id;
  expr.accept(*this);

  //  Pop global environment
  sym_table.pop_environment();
}

//  FunDecl visitor
void Interpreter::visit(FunDecl& node)
{
  //  Declare new FunDecl
  FunDecl* temp = new FunDecl;
  temp = &node;
  //  Add the key value pair of the function id and the node to the function map
  functions[node.id.lexeme()] = temp;
}

//  TypeDecl visitor
void Interpreter::visit(TypeDecl& node)
{
  //  Declare new TypeDecl
  TypeDecl* temp = new TypeDecl;
  temp = &node;
  //  Add the key value pair of the type id and the node to the type map
  types[node.id.lexeme()] = temp;
}

//  stmts
//  VarDeclStmt visitor
void Interpreter::visit(VarDeclStmt& node)
{
  //  Get curr type from the expression
  node.expr->accept(*this);
  //  Find type of curr_val
  if (curr_val.is_integer())
  {
    int v;
    curr_val.value(v);
    DataObject new_val(v);
    sym_table.add_name(node.id.lexeme());
    sym_table.set_val_info(node.id.lexeme(), new_val);
  }

  else if (curr_val.is_double())
  {
    double v;
    curr_val.value(v);
    DataObject new_val(v);
    sym_table.add_name(node.id.lexeme());
    sym_table.set_val_info(node.id.lexeme(), new_val);
  }

  else if (curr_val.is_char())
  {
    char v;
    curr_val.value(v);
    DataObject new_val(v);
    sym_table.add_name(node.id.lexeme());
    sym_table.set_val_info(node.id.lexeme(), new_val);
  }

  else if (curr_val.is_string())
  {
    std::string v;
    curr_val.value(v);
    DataObject new_val(v);
    sym_table.add_name(node.id.lexeme());
    sym_table.set_val_info(node.id.lexeme(), new_val);
  }

  else if (curr_val.is_bool())
  {
    bool v;
    curr_val.value(v);
    DataObject new_val(v);
    sym_table.add_name(node.id.lexeme());
    sym_table.set_val_info(node.id.lexeme(), new_val);
  }

  else if (curr_val.is_nil())
  {
    //  Set the data object to nil
    DataObject new_val = new DataObject();
    new_val.set_nil();
    sym_table.add_name(node.id.lexeme());
    sym_table.set_val_info(node.id.lexeme(), new_val);
  }
}

//  AssignStmt visitor
void Interpreter::visit(AssignStmt& node)
{
  //  Get value of the right hand side type
  node.expr->accept(*this);

  //  Treat the assignment as an idr val
  if (node.lvalue_list.size() > 1)
  {
    DataObject head_val;
    if (sym_table.has_val_info(node.lvalue_list.front().lexeme()))
      sym_table.get_val_info(node.lvalue_list.front().lexeme(), head_val);

    //  Check for assignment into a var that is defined as nil
    if (head_val.is_nil())
      error ("Runtime Error| Cannot assign into a value that is declared as nil");

    //  Copy token path for iteration and pop head element
    std::list<Token> node_path = node.lvalue_list;
    node_path.pop_front();

    while (node_path.size() > 1)
    {
      //  Get value of heap obj
      HeapObject obj;
      size_t oid;
      head_val.value(oid);
      heap.get_obj(oid, obj);

      //  Check if the object has a value stored in the heap
      if (obj.has_att(node_path.front().lexeme()))
        obj.get_val(node_path.front().lexeme(), head_val);
      node_path.pop_front();
    }

    HeapObject new_val;
    size_t new_oid;
    head_val.value(new_oid);
    heap.get_obj(new_oid, new_val);
    if (new_val.has_att(node_path.front().lexeme()))
      new_val.set_att(node_path.front().lexeme(), curr_val);
  }

  //  Assign into the a single variable
  else
    sym_table.set_val_info(node.lvalue_list.front().lexeme(), curr_val);
}

//  Return stmt visitor
void Interpreter::visit(ReturnStmt& node)
{
  node.expr->accept(*this);
  throw new MyPLReturnException;
}

//  If Stmt visitor
void Interpreter::visit(IfStmt& node)
{
  node.if_part->expr->accept(*this);
  bool val;
  curr_val.value(val);

  //  Trigger flag for the conditional statement
  bool condition = false;
  if (val)
  {
    sym_table.push_environment();
    for (Stmt* s : node.if_part->stmts)
      s->accept(*this);
    sym_table.pop_environment();
    condition = true;
  }

  //  Else-ifs (if they exist)
  if (node.else_ifs.size() > 0)
  {
    std::list<BasicIf*> elseifs = node.else_ifs;
    while (condition == false && elseifs.size() > 0)
    {
      BasicIf* new_if = elseifs.front();
      new_if->expr->accept(*this);
      curr_val.value(val);
      if (val)
      {
        sym_table.push_environment();
        for (Stmt* s : new_if->stmts)
          s->accept(*this);
        sym_table.pop_environment();
        condition = true;
      }
    }
  }

  //  Else (if it exists)
  if (condition == false && node.body_stmts.size() > 0)
  {
    sym_table.push_environment();
    for (Stmt* s : node.body_stmts)
      s->accept(*this);
    sym_table.pop_environment();
    condition = true;
  }
}

//  WhileStmt visitor
void Interpreter::visit(WhileStmt& node)
{
  //  Get value of the boolean loop continue flag
  node.expr->accept(*this);
  bool val;
  curr_val.value(val);
  sym_table.push_environment();

  //  Get value of each stmt in the while loop while the loop should continue
  while (val)
  {
    //  Get value of each statement
    for (Stmt* s : node.stmts)
      s->accept(*this);

    //  Check if the loop should continue
    node.expr->accept(*this);
    curr_val.value(val);
  }
  sym_table.pop_environment();
}

//  ForStmt visitor
void Interpreter::visit(ForStmt& node)
{
  int start_val;
  int end_val;
  //  Push new environment and add loop var to sym table
  sym_table.push_environment();
  node.start->accept(*this);
  //  Add loop var's value to the symbol table
  curr_val.value(start_val);
  sym_table.add_name(node.var_id.lexeme());
  sym_table.set_val_info(node.var_id.lexeme(), curr_val);
  node.end->accept(*this);
  //  Evaluate end expr
  curr_val.value(end_val);

  //  Push loop environment
  sym_table.push_environment();
  for (int i = start_val; i < end_val; i++)
  {
    //  All statements in loop to be iterated through
    std::list<Stmt*> stmt_list = node.stmts;
    for (Stmt* s: stmt_list)
      s->accept(*this);
  }
  //  Pop loop environment
  sym_table.pop_environment();
  //  Pop new ForStmt environment
  sym_table.pop_environment();
}

//  Final project declarations

void Interpreter::visit(TryStmt& node)
{
  //  Flag to indicate an error was triggered or a value was thrown
  bool flag_raised = false;
  bool raise_bool = false;
  bool raise_int = false;
  bool raise_double = false;
  //  If a catch stmt exists, store the value of the catch expression and ensure it isnt reached
  DataObject catch_val;

  //  Check if catch stmt exists
  if (node.catch_stmt->catch_expr)
  {
    //  Get value of catch stmt
    node.catch_stmt->catch_expr->accept(*this);
    //  If the value is an integer, store it
    if (curr_val.is_integer())
    {
      raise_int = true;
      int val;
      curr_val.value(val);
      catch_val.set(val);
    }

    //  If the value is an double, store it
    else if(curr_val.is_double())
    {
      raise_double = true;
      double val;
      curr_val.value(val);
      catch_val.set(val);
    }

    //  If the value is an boolean, store it
    else if(curr_val.is_bool())
    {
      raise_bool = true;
      bool val;
      curr_val.value(val);
      catch_val.set(val);
    }
  }

  //  Iterate through stmts and check that a the throw value isn't ever thrown, or
  //  the specified error is reached

  //  push a temp environment
  sym_table.push_environment();
  //  Copy all stmts
  std::list<Stmt*> try_stmts = node.stmts;
  //  Iterate through all stmts
  while (try_stmts.size() > 0)
  {
    try
    {
      try_stmts.front()->accept(*this);
    } catch (MyPLException e) {
      //  If an exception exists, check that it matches the id of the error in CatchStmt
      //  then stop iterating if it is the same
      flag_raised = true;
      if (e.get_type() == ZERODIVISION && node.catch_stmt->error_id.lexeme() == "ZeroDivision")
        break;
      else if (e.get_type() == INDEXOUTOFBOUNDS && node.catch_stmt->error_id.lexeme() == "IndexOutOfBounds")
        break;
      else
        throw e;
    }

    //  Check if the int value is equal to the value to be raised
    if (raise_int && curr_val.is_integer())
    {
      int cth_val;
      int stmt_val;
      catch_val.value(cth_val);
      curr_val.value(stmt_val);

      //  If it is, break the evaluation loop and move to catch stmt handling
      if (cth_val == stmt_val)
      {
        flag_raised = true;
        break;
      }
    }

    //  Check if the double value is equal to the value to be raised
    if (raise_double && curr_val.is_double())
    {
      double cth_val;
      double stmt_val;
      catch_val.value(cth_val);
      curr_val.value(stmt_val);

      //  If it is, break the evaluation loop and move to catch stmt handling
      if (cth_val == stmt_val)
      {
        flag_raised = true;
        break;
      }
    }

    //  Check if the bool value is equal to the value to be raised
    if (raise_bool && curr_val.is_bool())
    {
      bool cth_val;
      bool stmt_val;
      catch_val.value(cth_val);
      curr_val.value(stmt_val);

      //  If it is, break the evaluation loop and move to catch stmt handling
      if (cth_val == stmt_val)
      {
        flag_raised = true;
        break;
      }
    }

    try_stmts.pop_front();
  }
  sym_table.pop_environment();

  //  If an error was triggered properly or a value was raised that matches, evaluate the
  //  Catch stmt
  if (flag_raised)
    node.catch_stmt->accept(*this);
}

void Interpreter::visit(CatchStmt& node)
{
  sym_table.push_environment();
  for (Stmt* s : node.stmts)
    s->accept(*this);
  sym_table.pop_environment();
}

void Interpreter::visit(ThrowStmt& node)
{
  //  Get value of expression in throw stmt
  node.expr->accept(*this);
}

// expressions
//  Expr Visitor
void Interpreter::visit(Expr& node)
{
  //  Negated Expression
  if (node.negated)
  {
    //  Store negated expression
    node.first->accept(*this);
    bool val;
    curr_val.value(val);
    curr_val.set(!val);
  }

  //  Not negated
  else
  {
    //  Operation exists
    node.first->accept(*this);
    if (node.op)
    {
      DataObject lhs_val = curr_val;
      node.rest->accept(*this);
      DataObject rhs_val = curr_val;
      TokenType op = node.op->type();

      /*  Check all operators and their value cases */
      //  PLUS operator
      if (op == PLUS)
      {
        //  If integer --> curr_val = int + int
        if (lhs_val.is_integer())
        {
          if (rhs_val.is_nil())
            error("Runtime Error| Cannot assign nil value into a defined variable ");

          int x1;
          int x2;
          lhs_val.value(x1);
          rhs_val.value(x2);
          curr_val.set(x1+x2);
        }

        //  If double --> curr_val = double + double
        else if (lhs_val.is_double())
        {
          if (rhs_val.is_nil())
            error("Runtime Error| Cannot assign nil value into a defined variable ");

          double x1;
          double x2;
          lhs_val.value(x1);
          rhs_val.value(x2);
          curr_val.set(x1+x2);
        }

        //  String and char addition
        else
        {
          if (lhs_val.is_string() && rhs_val.is_string())
          {
            std::string x1;
            std::string x2;
            lhs_val.value(x1);
            rhs_val.value(x2);
            DataObject obj(x1+x2);
            curr_val = obj;
          }

          else if (lhs_val.is_string() && rhs_val.is_char())
          {
            std::string x1;
            char x2;
            lhs_val.value(x1);
            rhs_val.value(x2);
            DataObject obj(x1+x2);
            curr_val = obj;
          }

          else if (lhs_val.is_char() && rhs_val.is_string())
          {
            char x1;
            std::string x2;
            lhs_val.value(x1);
            rhs_val.value(x2);
            DataObject obj(x1+x2);
            curr_val = obj;
          }

          else if (lhs_val.is_char() && rhs_val.is_char())
          {
            char x1;
            char x2;
            lhs_val.value(x1);
            rhs_val.value(x2);
            DataObject obj(x1+x2);
            curr_val = obj;
          }
        }
      }

      //  MINUS operator
      else if (op == MINUS)
      {
        //  If integer --> curr_val = int - int
        if (lhs_val.is_integer())
        {
          if (rhs_val.is_nil())
            error("Runtime Error| Cannot assign nil value into a defined variable ");

          int x1;
          int x2;
          lhs_val.value(x1);
          rhs_val.value(x2);
          curr_val.set(x1-x2);
        }

        //  If double --> curr_val = double - double
        else if (lhs_val.is_double())
        {
          if (rhs_val.is_nil())
            error("Runtime Error| Cannot assign nil value into a defined variable ");

          double x1;
          double x2;
          lhs_val.value(x1);
          rhs_val.value(x2);
          curr_val.set(x1-x2);
        }
      }

      //  MULTIPLY operator
      else if (op == MULTIPLY)
      {
        //  If integer --> curr_val = int * int
        if (lhs_val.is_integer())
        {
          if (rhs_val.is_nil())
            error("Runtime Error| Cannot assign nil value into a defined variable ");

          int x1;
          int x2;
          lhs_val.value(x1);
          rhs_val.value(x2);
          curr_val.set(x1*x2);
        }

        //  If double --> curr_val = double * double
        else if (lhs_val.is_double())
        {
          if (rhs_val.is_nil())
            error("Runtime Error| Cannot assign nil value into a defined variable ");

          double x1;
          double x2;
          lhs_val.value(x1);
          rhs_val.value(x2);
          curr_val.set(x1*x2);
        }
      }

      //  DIVIDE operator
      else if (op == DIVIDE)
      {
        //  If integer --> curr_val = int / int
        if (lhs_val.is_integer())
        {
          if (rhs_val.is_nil())
            error("Runtime Error| Cannot assign nil value into a defined variable ");

          int x1;
          int x2;
          lhs_val.value(x1);
          rhs_val.value(x2);

          if (x2 == 0)
            zerodiv_error("Runtime Error| Division by Zero error");

          curr_val.set(x1/x2);
        }

        //  If double --> curr_val = double / double
        else if (lhs_val.is_double())
        {
          if (rhs_val.is_nil())
            error("Runtime Error| Cannot assign nil value into a defined variable ");

          double x1;
          double x2;
          lhs_val.value(x1);
          rhs_val.value(x2);

          if (x2 == 0.0)
            zerodiv_error("Runtime Error| Division by Zero error");

          curr_val.set(x1/x2);
        }
      }

      //  MODULO operator
      else if (op == MODULO)
      {
        if (rhs_val.is_nil())
          error("Runtime Error| Cannot assign nil value into a defined variable ");

        int x1;
        int x2;
        lhs_val.value(x1);
        rhs_val.value(x2);

        if (x2 == 0)
          zerodiv_error("Runtime Error| Division by Zero error");

        curr_val.set(x1%x2);
      }

      //  AND operator
      else if (op == AND)
      {
        bool x1;
        bool x2;
        lhs_val.value(x1);
        rhs_val.value(x2);
        DataObject obj((x1&&x2));
        curr_val = obj;
      }

      //  OR operator
      else if (op == OR)
      {
        bool x1;
        bool x2;
        lhs_val.value(x1);
        rhs_val.value(x2);
        DataObject obj((x1||x2));
        curr_val = obj;
      }

      //  EQUAL operator
      else if (op == EQUAL)
      {
        //  INTEGER -- > bool
        if (lhs_val.is_integer())
        {
          if (rhs_val.is_nil())
            error("Runtime Error| Cannot assign nil value into a defined variable ");

          int x1;
          int x2;
          lhs_val.value(x1);
          rhs_val.value(x2);
          DataObject obj(x1==x2);
          curr_val = obj;
        }

        //  DOUBLE --> bool
        else if (lhs_val.is_double())
        {
          if (rhs_val.is_nil())
            error("Runtime Error| Cannot assign nil value into a defined variable ");

          double x1;
          double x2;
          lhs_val.value(x1);
          rhs_val.value(x2);
          DataObject obj(x1==x2);
          curr_val = obj;
        }

        //  CHAR --> bool
        else if (lhs_val.is_char())
        {
          if (rhs_val.is_nil())
            error("Runtime Error| Cannot assign nil value into a defined variable ");

          char x1;
          char x2;
          lhs_val.value(x1);
          rhs_val.value(x2);
          DataObject obj(x1 == x2);
          curr_val = obj;
        }

        //  string --> bool
        else if (lhs_val.is_string())
        {
          if (rhs_val.is_nil())
            error("Runtime Error| Cannot assign nil value into a defined variable ");

          std::string x1;
          std::string x2;
          lhs_val.value(x1);
          rhs_val.value(x2);
          DataObject obj(x1==x2);
          curr_val = obj;
        }

        //  BOOL --> bool
        else if (lhs_val.is_bool())
        {
          if (rhs_val.is_nil())
            error("Runtime Error| Cannot assign nil value into a defined variable ");

          bool x1;
          bool x2;
          lhs_val.value(x1);
          rhs_val.value(x2);
          DataObject obj(x1 == x2);
          curr_val = obj;
        }

        //  UDT expressions
        //  OID --> bool
        else if (lhs_val.is_oid())
        {
          if (rhs_val.is_nil())
            error("Runtime Error| Cannot assign nil value into a defined variable ");

          size_t x1;
          size_t x2;
          lhs_val.value(x1);
          rhs_val.value(x2);
          DataObject obj(x1==x2);
          curr_val = obj;
        }
      }

      //  NOT_EQUAL operator
      else if (op == NOT_EQUAL)
      {
        //  INTEGER -- > bool
        if (lhs_val.is_integer())
        {
          if (rhs_val.is_nil())
            error("Runtime Error| Cannot assign nil value into a defined variable ");

          int x1;
          int x2;
          lhs_val.value(x1);
          rhs_val.value(x2);
          DataObject obj(x1!=x2);
          curr_val = obj;
        }

        //  DOUBLE --> bool
        else if (lhs_val.is_double())
        {
          if (rhs_val.is_nil())
            error("Runtime Error| Cannot assign nil value into a defined variable ");

          double x1;
          double x2;
          lhs_val.value(x1);
          rhs_val.value(x2);
          DataObject obj(x1 != x2);
          curr_val = obj;
        }

        //  CHAR --> bool
        else if (lhs_val.is_char())
        {
          if (rhs_val.is_nil())
            error("Runtime Error| Cannot assign nil value into a defined variable ");

          char x1;
          char x2;
          lhs_val.value(x1);
          rhs_val.value(x2);
          DataObject obj(x1 != x2);
          curr_val = obj;
        }

        //  string --> bool
        else if (lhs_val.is_string())
        {
          if (rhs_val.is_nil())
            error("Runtime Error| Cannot assign nil value into a defined variable ");

          std::string x1;
          std::string x2;
          lhs_val.value(x1);
          rhs_val.value(x2);
          DataObject obj(x1!=x2);
          curr_val = obj;
        }

        //  BOOL --> bool
        else if (lhs_val.is_bool())
        {
          if (rhs_val.is_nil())
            error("Runtime Error| Cannot assign nil value into a defined variable ");

          bool x1;
          bool x2;
          lhs_val.value(x1);
          rhs_val.value(x2);
          DataObject obj(x1!=x2);
          curr_val = obj;
        }

        //  UDT expressions
        //  OID --> bool
        else if (lhs_val.is_oid())
        {
          if (rhs_val.is_nil())
            error("Runtime Error| Cannot assign nil value into a defined variable ");

          size_t x1;
          size_t x2;
          lhs_val.value(x1);
          rhs_val.value(x2);
          DataObject obj(x1!=x2);
          curr_val = obj;
        }
      }

      //  LESS operator
      else if (op == LESS)
      {
        //  If integer --> bool
        if (lhs_val.is_integer())
        {
          if (rhs_val.is_nil())
            error("Runtime Error| Cannot assign nil value into a defined variable ");

          int x1;
          int x2;
          lhs_val.value(x1);
          rhs_val.value(x2);
          DataObject obj(x1<x2);
          curr_val = obj;
        }

        //  If double --> bool
        else if (lhs_val.is_double())
        {
          if (rhs_val.is_nil())
            error("Runtime Error| Cannot assign nil value into a defined variable ");

          double x1;
          double x2;
          lhs_val.value(x1);
          rhs_val.value(x2);
          DataObject obj(x1<x2);
          curr_val = obj;
        }
      }

      //  LESS_EQUAL operator
      else if (op == LESS_EQUAL)
      {
        //  If integer --> bool
        if (lhs_val.is_integer())
        {
          if (rhs_val.is_nil())
            error("Runtime Error| Cannot assign nil value into a defined variable ");

          int x1;
          int x2;
          lhs_val.value(x1);
          rhs_val.value(x2);
          DataObject obj(x1<=x2);
          curr_val = obj;
        }

        //  If double --> bool
        else if (lhs_val.is_double())
        {
          if (rhs_val.is_nil())
            error("Runtime Error| Cannot assign nil value into a defined variable ");

          double x1;
          double x2;
          lhs_val.value(x1);
          rhs_val.value(x2);
          DataObject obj(x1<=x2);
          curr_val = obj;
        }
      }

      //  GREATER operator
      else if (op == GREATER)
      {
        //  If integer --> bool
        if (lhs_val.is_integer())
        {
          if (rhs_val.is_nil())
            error("Runtime Error| Cannot assign nil value into a defined variable ");

          int x1;
          int x2;
          lhs_val.value(x1);
          rhs_val.value(x2);
          DataObject obj(x1>x2);
          curr_val = obj;
        }

        //  If double --> bool
        else if (lhs_val.is_double())
        {
          if (rhs_val.is_nil())
            error("Runtime Error| Cannot assign nil value into a defined variable ");

          double x1;
          double x2;
          lhs_val.value(x1);
          rhs_val.value(x2);
          DataObject obj(x1>x2);
          curr_val = obj;
        }
      }

      //  GREATER_EQUAL operator
      else if (op == GREATER_EQUAL)
      {
        //  If integer --> bool
        if (lhs_val.is_integer())
        {
          if (rhs_val.is_nil())
            error("Runtime Error| Cannot assign nil value into a defined variable ");

          int x1;
          int x2;
          lhs_val.value(x1);
          rhs_val.value(x2);
          DataObject obj(x1>=x2);
          curr_val = obj;
        }

        //  If double --> bool
        else if (lhs_val.is_double())
        {
          if (rhs_val.is_nil())
            error("Runtime Error| Cannot assign nil value into a defined variable ");

          double x1;
          double x2;
          lhs_val.value(x1);
          rhs_val.value(x2);
          DataObject obj(x1>=x2);
          curr_val = obj;
        }
      }
    }
  }
}

//  SimpleTerm Visitor
void Interpreter::visit(SimpleTerm& node)
{
  //  Get value of rval in curr_val
  node.rvalue->accept(*this);
}

//  ComplexTerm
void Interpreter::visit(ComplexTerm& node)
{
  //  Get value of expr in curr_val
  node.expr->accept(*this);
}

// rvalues
//  SimpleRValue visitor implementation
void Interpreter::visit(SimpleRValue& node)
{
  switch (node.value.type())
  {
    case CHAR_VAL:
      curr_val.set(node.value.lexeme().at(0));
      break;
    case STRING_VAL:
      curr_val.set(node.value.lexeme());
      break;
    case INT_VAL:
      curr_val.set(std::stoi(node.value.lexeme()));
      break;
    case DOUBLE_VAL:
      curr_val.set(std::stod(node.value.lexeme()));
      break;
    case BOOL_VAL:
    {
      if (node.value.lexeme() == "true")
        curr_val.set(true);
      else
        curr_val.set(false);

      break;
    }

    default:
      curr_val.set_nil();
      break;
  }
}

//  NewRValue visitor
void Interpreter::visit(NewRValue& node)
{
  //  set curr_val to value of the new UDT type being declared
  TypeDecl* udt = types[node.type_id.lexeme()];
  HeapObject new_udt_decl;

  //  iterate through var decls in udt and create new data object for
  //  each decl in the udt implementation
  for (VarDeclStmt* decl : udt->vdecls)
  {
    //  Get type of expr and put it in curr_val
    decl->expr->accept(*this);
    DataObject new_obj = DataObject(curr_val);
    //  Get next oid and add heap obj to heap
    heap.set_obj(next_oid, new_udt_decl);
    next_oid++;
  }

  //  Set current value to oid of heap object
  curr_val.set(next_oid);
}

//  CallExpr visitor
void Interpreter::visit(CallExpr& node)
{
  std::string fun_name = node.function_id.lexeme();

  //  Built-in functions
  //  print
  if (fun_name == "print")
  {
    //  Null function so no value needs to be stored
    node.arg_list.front()->accept(*this);
    std::string s = curr_val.to_string();
    //  Regex replace \n and \t chars
    s = std::regex_replace(s, std::regex("\\\\n"), "\n");
    s = std::regex_replace(s, std::regex("\\\\t"), "\t");
    std::cout << s << std::endl;
  }

  //  read
  else if (fun_name == "read")
  {
    //  Non-null function, save value into curr_value
    std::string user_input;
    std::cin >> user_input;
    DataObject obj(user_input);
    curr_val = obj;
  }

  //  stoi
  else if (fun_name == "stoi")
  {
    //  Retrieve string and convert it to an integer
    node.arg_list.front()->accept(*this);
    std::string s;
    curr_val.value(s);
    int new_int = std::stoi(s);
    DataObject obj(new_int);
    curr_val = obj;
  }

  // itos
  else if (fun_name == "itos")
  {
    //  Retrive integer and convert it to a string
    node.arg_list.front()->accept(*this);
    std::string new_str = curr_val.to_string();
    DataObject obj(new_str);
    curr_val = obj;
  }

  //  stod
  else if (fun_name == "stod")
  {
    //  Retrieve string and turn it into double
    node.arg_list.front()->accept(*this);
    std::string s;
    curr_val.value(s);
    double new_double = std::stod(s);
    DataObject obj(new_double);
    curr_val = obj;
  }

  //  dtos
  else if (fun_name == "dtos")
  {
    //  Retrieve double and convert it to a string
    node.arg_list.front()->accept(*this);
    std::string new_str = curr_val.to_string();
    DataObject obj(new_str);
    curr_val = obj;
  }

  //  get
  else if (fun_name == "get")
  {
    std::list<Expr*> expr_list = node.arg_list;
    //  Get value of index and store it
    expr_list.front()->accept(*this);
    int idx;
    curr_val.value(idx);
    expr_list.pop_front();
    //  Get string that needs to be operated on and store it
    expr_list.front()->accept(*this);
    std::string str_val;
    curr_val.value(str_val);

    int str_len = str_val.length();

    if (idx < 0 || idx > str_len-1)
      index_error("Runtime Error| Index out of Bounds");

    //  Get char at index and store it in curr value
    DataObject obj(str_val[idx]);
    curr_val = obj;
  }

  //  length
  else if (fun_name == "length")
  {
    node.arg_list.front()->accept(*this);
    std::string curr_str;
    curr_val.value(curr_str);
    DataObject obj(curr_str.length());
    curr_val = obj;
  }

  //  All other function calls
  else
  {
    FunDecl* fun_node = functions[fun_name];
    std::list<Expr*> expr_list = node.arg_list;
    std::list<DataObject> params;
    //  Iterate through all params, create a new dataobject for each, and then
    //  add them to the list
    while (expr_list.size() != 0)
    {
      //  Get value of expr in curr_val and add them top the param list
      Expr* e = expr_list.front();
      e->accept(*this);
      DataObject new_param = curr_val;
      params.push_back(new_param);
      expr_list.pop_front();
    }

    //  Save curr environment
    int old_env = sym_table.get_environment_id();
    //  Go to global environment
    sym_table.set_environment_id(global_env_id);
    sym_table.push_environment();
    FunDecl* function = functions[fun_name];
    for (FunDecl::FunParam param : function->params)
    {
      //  Get name of param, then add the id and corresponding dataobj to env
      sym_table.add_name(param.id.lexeme());
      sym_table.set_val_info(param.id.lexeme(), params.front());
      params.pop_front();
    }

    //  Evaluate each statement in the function and check for return code
    try {
      for (Stmt* s : fun_node->stmts)
        s->accept(*this);
    }
    catch (MyPLReturnException* r)
    {
      //  If the function name is main, store val of curr_val in ret_code
      if (fun_name == "main")
        curr_val.value(ret_code);
    }

    sym_table.pop_environment();
    //  Return to old environment
    sym_table.set_environment_id(old_env);
  }
}

//  IDRValue visitor
void Interpreter::visit(IDRValue& node)
{
  //  IDRValue path exists
  if (node.path.size() > 1)
  {
    std::list<Token> path = node.path;
    DataObject curr_object;
    //  Get oid of object in the head of the path
    if (sym_table.has_val_info(path.front().lexeme()))
      sym_table.get_val_info(path.front().lexeme(), curr_object);

    //  Remove first token from the list
    path.pop_front();
    //  Iterate through all nodes in path
    while (path.size() > 1)
    {
      HeapObject obj;
      size_t oid;
      curr_object.value(oid);
      if (obj.has_att(path.front().lexeme()))
        obj.get_val(path.front().lexeme(), curr_object);
      path.pop_front();
    }

    //  Set curr_val to the value of the path object
    HeapObject obj;
    size_t oid;
    curr_object.value(oid);
    heap.get_obj(oid, obj);
    if (obj.has_att(path.front().lexeme()))
    {
      DataObject value;
      obj.get_val(path.front().lexeme(), value);
      curr_val = value;
    }
  }

  //  Single IDRValue object
  else
  {
    if (sym_table.has_val_info(node.path.front().lexeme()))
    {
      DataObject curr_object;
      sym_table.get_val_info(node.path.front().lexeme(), curr_object);
      curr_val = curr_object;
    }
  }
}

//  Negated R Value
void Interpreter::visit(NegatedRValue& node)
{
  //  Get value of expr in curr_val
  node.expr->accept(*this);

  //  If value is an int
  if (curr_val.is_integer())
  {
    int val;
    curr_val.value(val);
    curr_val.set(val * -1);
  }

  else
  {
     double val;
     curr_val.value(val);
     curr_val.set(val * -1.0);
  }
}

#endif
