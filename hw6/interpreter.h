
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

  sym_table.pop_environment();
}

//  FunDecl visitor
void Interpreter::visit(FunDecl& node)
{
  //  Add the key value pair of the function id and the node to the function map
  functions[node.id.lexeme()] = &node;
}

//  TypeDecl visitor
void Interpreter::visit(TypeDecl& node)
{
  //  Add the key value pair of the type id and the node to the type map
  types[node.id.lexeme()] = &node;
}

//  stmts
//  VarDeclStmt visitor
void Interpreter::visit(VarDeclStmt& node)
{
  //  Get curr type from the expression
  node.expr->accept(*this);
  //  Assign the id to the curr_val
  sym_table.set_val_info(node.id.lexeme(), curr_val);
}

//  AssignStmt visitor
void Interpreter::visit(AssignStmt& node)
{
  //  Get value of the right hand side type
  node.expr->accept(*this);

  //  Treat the assignment as an idr val
  if (node.lvalue_list.size() > 1)
  {
    HeapObject type;
    DataObject head_val;
    sym_table.get_val_info(node.lvalue_list.front().lexeme(), head_val);

    //  Check for assignment into a var that is defined as nil
    if (head_val.is_nil())
      error ("cannot assign into a value that is declared as nil");

    //  Get oid from type of the node
    size_t type_oid;
    head_val.value(type_oid);
    heap.get_obj(type_oid, type);

    //  Copy token path for iteration and pop head element
    std::list<Token> node_path = node.lvalue_list;
    node_path.pop_front();

    while (node_path.size() > 1)
    {
      //  Iterate through path and and ensure that no values are nil
      if (!type.has_att(node_path.front().lexeme()));
        error("Attribute defined as nil");

      node_path.pop_front();
    }

    //  Modify heap object with the value
    type.set_att(node_path.front().lexeme(), curr_val);
  }

  //  Assign into the a single variable
  else
  {
    //  Get value of expr
    DataObject rhs;
    node.expr->accept(*this);
    //  Set its value to curr_val in symbol table
    sym_table.set_val_info(node.lvalue_list.front().lexeme(), curr_val);
  }
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
  if (node.if_part)
  {
    //  Type check value of if stmt expression
    node.if_part->expr->accept(*this);

    sym_table.push_environment();
    for (Stmt* s : node.if_part->stmts)
      s->accept(*this);
    sym_table.pop_environment();

    //  Check if else if's exists and check the values of each statement in them
    if (node.else_ifs.size() > 0)
    {
      for (BasicIf* elifs : node.else_ifs)
      {
        elifs->expr->accept(*this);

        sym_table.push_environment();
        for (Stmt* s : elifs->stmts)
          s->accept(*this);
        sym_table.pop_environment();
      }
    }

    if (node.body_stmts.size() > 0)
    {
      sym_table.push_environment();
      for (Stmt* s : node.body_stmts)
        s->accept(*this);
      sym_table.pop_environment();
    }
  }

  else
    error("If statement must have a body ");
}

//  WhileStmt visitor
void Interpreter::visit(WhileStmt& node)
{
  //  check boolean expr and ensure it is not a nil var
  node.expr->accept(*this);
  if (curr_val.is_nil())
    error("Cannot base while loop expression around nil value ");

  //  Get value of each stmt in the while loop
  sym_table.push_environment();
  for (Stmt* s : node.stmts)
    s->accept(*this);
  sym_table.pop_environment();
}

//  ForStmt visitor
void Interpreter::visit(ForStmt& node)
{
  int start_val;
  int end_val;
  //  Push new environment and add loop var to sym table
  sym_table.push_environment();
  sym_table.add_name(node.var_id.lexeme());
  node.start->accept(*this);
  //  Add loop var's value to the symbol table
  curr_val.value(start_val);
  sym_table.set_val_info(node.var_id.lexeme(), curr_val);

  //  Evaluate end expr
  curr_val.value(end_val);

  while (end_val > start_val)
  {
    sym_table.push_environment();
    for (Stmt* s : node.stmts)
      s->accept(*this);
    sym_table.pop_environment();

    int new_loop_iter;
    DataObject loop_var;
    sym_table.get_val_info(node.var_id.lexeme(), loop_var);
    loop_var.value(new_loop_iter);
    loop_var.set(new_loop_iter+1);
    sym_table.set_val_info(node.var_id.lexeme(), loop_var);
    end_val --;
  }
  sym_table.pop_environment();
}

// expressions
//  Expr Visitor
void Interpreter::visit(Expr& node)
{
  //  Negated Expression
  if (node.negated)
  {
    node.first->accept(*this);
    bool val;
    curr_val.value(val);
    curr_val.set(!val);
  }

  //  Not negated
  else
  {
    //  Operation exists
    if (node.op)
    {
      node.first->accept(*this);
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
            error("Cannot assign nil value into a defined variable ");

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
            error("Cannot assign nil value into a defined variable ");

          double x1;
          double x2;
          lhs_val.value(x1);
          rhs_val.value(x2);
          curr_val.set(x1+x2);
        }

        //  If string --> curr_val = string + char
        else if (lhs_val.is_string())
        {
          if (rhs_val.is_nil())
            error("Cannot assign nil value into a defined variable ");

          string x1;
          char x2;
          lhs_val.value(x1);
          rhs_val.value(x2);
          curr_val.set(x1+x2);
        }

        //  If char --> curr_val = char + char = string
        else if (lhs_val.is_char())
        {
          if (rhs_val.is_nil())
            error("Cannot assign nil value into a defined variable ");

          string x1;
          char x2;
          lhs_val.value(x1);
          rhs_val.value(x2);
          curr_val.set(x1+x2);
        }
      }

      //  MINUS operator
      else if (op == MINUS)
      {
        //  If integer --> curr_val = int - int
        if (lhs_val.is_integer())
        {
          if (rhs_val.is_nil())
            error("Cannot assign nil value into a defined variable ");

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
            error("Cannot assign nil value into a defined variable ");

          double x1;
          double x2;
          lhs_val.value(x1);
          rhs_val.value(x2);
          curr_val.set(x1-x2);
        }
      }

      //  DIVIDE operator
      else if (op == DIVIDE)
      {
        //  If integer --> curr_val = int / int
        if (lhs_val.is_integer())
        {
          if (rhs_val.is_nil())
            error("Cannot assign nil value into a defined variable ");

          int x1;
          int x2;
          lhs_val.value(x1);
          rhs_val.value(x2);
          curr_val.set(x1/x2);
        }

        //  If double --> curr_val = double / double
        else if (lhs_val.is_double())
        {
          if (rhs_val.is_nil())
            error("Cannot assign nil value into a defined variable ");

          double x1;
          double x2;
          lhs_val.value(x1);
          rhs_val.value(x2);
          curr_val.set(x1/x2);
        }
      }

      //  MULTIPLY operator
      else if (op == MULTIPLY)
      {
        //  If integer --> curr_val = int * int
        if (lhs_val.is_integer())
        {
          if (rhs_val.is_nil())
            error("Cannot assign nil value into a defined variable ");

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
            error("Cannot assign nil value into a defined variable ");

          double x1;
          double x2;
          lhs_val.value(x1);
          rhs_val.value(x2);
          curr_val.set(x1*x2);
        }
      }

      //  MODULO operator
      else if (op == MODULO)
      {
        //  If integer --> curr_val = int + int
        if (lhs_val.is_integer())
        {
          if (rhs_val.is_nil())
            error("Cannot assign nil value into a defined variable ");

          int x1;
          int x2;
          lhs_val.value(x1);
          rhs_val.value(x2);
          curr_val.set(x1*x2);
        }
      }

      //  AND operator
      else if (op == AND)
      {
        if (rhs_val.is_nil())
          error("Cannot assign nil value into a defined variable ");

        bool x1;
        bool x2;
        lhs_val.value(x1);
        rhs_val.value(x2);
        bool result = x1 && x2;
        curr_val.set(result);
      }

      //  OR operator
      else if (op == OR)
      {
        if (rhs_val.is_nil())
          error("Cannot assign nil value into a defined variable ");

        bool x1;
        bool x2;
        lhs_val.value(x1);
        rhs_val.value(x2);
        bool result = x1 && x2;
        curr_val.set(result);
      }

      //  EQUAL operator
      else if (op == EQUAL)
      {
        //  INTEGER -- > bool
        if (lhs_val.is_integer())
        {
          if (rhs_val.is_nil())
            error("Cannot assign nil value into a defined variable ");

          int x1;
          int x2;
          bool result;
          lhs_val.value(x1);
          rhs_val.value(x2);
          result = (x1 == x2);
          curr_val.set(result);
        }

        //  DOUBLE --> bool
        else if (lhs_val.is_double())
        {
          if (rhs_val.is_nil())
            error("Cannot assign nil value into a defined variable ");

          double x1;
          double x2;
          bool result;
          lhs_val.value(x1);
          rhs_val.value(x2);
          result = (x1 == x2);
          curr_val.set(result);
        }

        //  CHAR --> bool
        else if (lhs_val.is_char())
        {
          if (rhs_val.is_nil())
            error("Cannot assign nil value into a defined variable ");

          char x1;
          char x2;
          bool result;
          lhs_val.value(x1);
          rhs_val.value(x2);
          result = (x1 == x2);
          curr_val.set(result);
        }

        //  string --> bool
        else if (lhs_val.is_string())
        {
          if (rhs_val.is_nil())
            error("Cannot assign nil value into a defined variable ");

          string x1;
          string x2;
          bool result;
          lhs_val.value(x1);
          rhs_val.value(x2);
          result = (x1 == x2);
          curr_val.set(result);
        }

        //  BOOL --> bool
        else if (lhs_val.is_bool())
        {
          if (rhs_val.is_nil())
            error("Cannot assign nil value into a defined variable ");

          bool x1;
          bool x2;
          bool result;
          lhs_val.value(x1);
          rhs_val.value(x2);
          result = (x1 == x2);
          curr_val.set(result);
        }

        //  UDT expressions
        //  OID --> bool
        else if (lhs_val.is_oid())
        {
          if (rhs_val.is_nil())
            error("Cannot assign nil value into a defined variable ");

          size_t x1;
          size_t x2;
          bool result;
          lhs_val.value(x1);
          rhs_val.value(x2);
          result = (x1 == x2);
          curr_val.set(result);
        }
      }

      //  NOT_EQUAL operator
      else if (op == NOT_EQUAL)
      {
        //  INTEGER -- > bool
        if (lhs_val.is_integer())
        {
          if (rhs_val.is_nil())
            error("Cannot assign nil value into a defined variable ");

          int x1;
          int x2;
          bool result;
          lhs_val.value(x1);
          rhs_val.value(x2);
          result = (x1 != x2);
          curr_val.set(result);
        }

        //  DOUBLE --> bool
        else if (lhs_val.is_double())
        {
          if (rhs_val.is_nil())
            error("Cannot assign nil value into a defined variable ");

          double x1;
          double x2;
          bool result;
          lhs_val.value(x1);
          rhs_val.value(x2);
          result = (x1 != x2);
          curr_val.set(result);
        }

        //  CHAR --> bool
        else if (lhs_val.is_char())
        {
          if (rhs_val.is_nil())
            error("Cannot assign nil value into a defined variable ");

          char x1;
          char x2;
          bool result;
          lhs_val.value(x1);
          rhs_val.value(x2);
          result = (x1 != x2);
          curr_val.set(result);
        }

        //  string --> bool
        else if (lhs_val.is_string())
        {
          if (rhs_val.is_nil())
            error("Cannot assign nil value into a defined variable ");

          string x1;
          string x2;
          bool result;
          lhs_val.value(x1);
          rhs_val.value(x2);
          result = (x1 != x2);
          curr_val.set(result);
        }

        //  BOOL --> bool
        else if (lhs_val.is_bool())
        {
          if (rhs_val.is_nil())
            error("Cannot assign nil value into a defined variable ");

          bool x1;
          bool x2;
          bool result;
          lhs_val.value(x1);
          rhs_val.value(x2);
          result = (x1 != x2);
          curr_val.set(result);
        }

        //  UDT expressions
        //  OID --> bool
        else if (lhs_val.is_oid())
        {
          if (rhs_val.is_nil())
            error("Cannot assign nil value into a defined variable ");

          size_t x1;
          size_t x2;
          bool result;
          lhs_val.value(x1);
          rhs_val.value(x2);
          result = (x1 != x2);
          curr_val.set(result);
        }
      }

      //  LESS operator
      else if (op == LESS)
      {
        //  If integer --> bool
        if (lhs_val.is_integer())
        {
          if (rhs_val.is_nil())
            error("Cannot assign nil value into a defined variable ");

          int x1;
          int x2;
          bool result;
          lhs_val.value(x1);
          rhs_val.value(x2);
          result = (x1 < x2);
          curr_val.set(result);
        }

        //  If double --> bool
        else if (lhs_val.is_double())
        {
          if (rhs_val.is_nil())
            error("Cannot assign nil value into a defined variable ");

          double x1;
          double x2;
          bool result;
          lhs_val.value(x1);
          rhs_val.value(x2);
          result = (x1 < x2);
          curr_val.set(result);
        }
      }

      //  LESS_EQUAL operator
      else if (op == LESS_EQUAL)
      {
        //  If integer --> bool
        if (lhs_val.is_integer())
        {
          if (rhs_val.is_nil())
            error("Cannot assign nil value into a defined variable ");

          int x1;
          int x2;
          bool result;
          lhs_val.value(x1);
          rhs_val.value(x2);
          result = (x1 <= x2);
          curr_val.set(result);
        }

        //  If double --> bool
        else if (lhs_val.is_double())
        {
          if (rhs_val.is_nil())
            error("Cannot assign nil value into a defined variable ");

          double x1;
          double x2;
          bool result;
          lhs_val.value(x1);
          rhs_val.value(x2);
          result = (x1 <= x2);
          curr_val.set(result);
        }
      }

      //  GREATER operator
      else if (op == GREATER)
      {
        //  If integer --> bool
        if (lhs_val.is_integer())
        {
          if (rhs_val.is_nil())
            error("Cannot assign nil value into a defined variable ");

          int x1;
          int x2;
          bool result;
          lhs_val.value(x1);
          rhs_val.value(x2);
          result = (x1 > x2);
          curr_val.set(result);
        }

        //  If double --> bool
        else if (lhs_val.is_double())
        {
          if (rhs_val.is_nil())
            error("Cannot assign nil value into a defined variable ");

          double x1;
          double x2;
          bool result;
          lhs_val.value(x1);
          rhs_val.value(x2);
          result = (x1 > x2);
          curr_val.set(result);
        }
      }

      //  GREATER_EQUAL operator
      else if (op == GREATER_EQUAL)
      {
        //  If integer --> bool
        if (lhs_val.is_integer())
        {
          if (rhs_val.is_nil())
            error("Cannot assign nil value into a defined variable ");

          int x1;
          int x2;
          bool result;
          lhs_val.value(x1);
          rhs_val.value(x2);
          result = (x1 >= x2);
          curr_val.set(result);
        }

        //  If double --> bool
        else if (lhs_val.is_double())
        {
          if (rhs_val.is_nil())
            error("Cannot assign nil value into a defined variable ");

          double x1;
          double x2;
          bool result;
          lhs_val.value(x1);
          rhs_val.value(x2);
          result = (x1 >= x2);
          curr_val.set(result);
        }
      }
    }

    else
      node.first->accept(*this);
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
    next_oid++;
    heap.set_obj(next_oid, new_udt_decl);
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
    std::cout << s;
  }

  //  read
  else if (fun_name == "read")
  {
    //  Non-null function, save value into curr_value
    std::string user_input;
    cin >> user_input;
    curr_val.set(user_input);
  }

  //  stoi
  else if (fun_name == "stoi")
  {
    //  Retrieve string and convert it to an integer
    node.arg_list.front()->accept(*this);
    std::string s;
    curr_val.value(s);
    int new_int = std::stoi(s);
    curr_val.set(new_int);
  }

  // itos
  else if (fun_name == "itos")
  {
    //  Retrive integer and convert it to a string
    node.arg_list.front()->accept(*this);
    int i;
    curr_val.value(i);
    std::string new_str = std::to_string(i);
    curr_val.set(new_str);
  }

  //  stod
  else if (fun_name == "stod")
  {
    //  Retrieve string and turn it into double
    node.arg_list.front()->accept(*this);
    std::string s;
    curr_val.value(s);
    double new_double = std::stod(s);
    curr_val.set(new_double);
  }

  //  dtos
  else if (fun_name == "dtos")
  {
    //  Retrieve double and convert it to a string
    node.arg_list.front()->accept(*this);
    double d;
    curr_val.value(d);
    std::string new_str = std::to_string(d);
  }

  //  get
  else if (fun_name == "get")
  {
    //  Get value of index and store it
    node.arg_list.front()->accept(*this);
    int idx;
    curr_val.value(idx);

    //  Get string that needs to be operated on and store it
    node.arg_list.front()->accept(*this);
    std::string str_val;
    curr_val.value(str_val);

    int str_len = str_val.length();

    if (idx < 0 || idx > str_len-1)
      error("Index out of bounds for string of length "+str_len);

    char new_char = str_val[idx];
    curr_val.set(new_char);
  }

  //  length
  else if (fun_name == "length")
  {
    node.arg_list.front()->accept(*this);
    std::string curr_str;
    curr_val.value(curr_str);
    curr_val.set(curr_str.length());
  }

  //  All other function calls
  else
  {
    FunDecl* fun_node = functions[fun_name];
    std::list<DataObject> params;
    //  Iterate through all params, create a new dataobject for each, and then
    //  add them to the list
    for (Expr* expr : node.arg_list)
    {
      //  Get value of expr in curr_val and add them top the param list
      expr->accept(*this);
      DataObject new_param = curr_val;
      params.push_back(new_param);
    }

    //  Save curr environment
    int old_env = sym_table.get_environment_id();
    //  Go to global environment
    sym_table.set_environment_id(0);
    sym_table.push_environment();

    //  Copy params then add param values to environment
    std::list<DataObject> new_params = params;
    std::list<FunDecl::FunParam> fun_params = fun_node->params;
    while (fun_params.size() > 0)
    {
      //  Get name of param, then add the id and corresponding dataobj to env
      std::string param_id = fun_params.front().id.lexeme();
      sym_table.set_val_info(param_id, new_params.front());
      new_params.pop_front();
      fun_params.pop_front();
    }

    //  Evaluate each statement in the function
    try {
      for (Stmt* s : fun_node->stmts)
        s->accept(*this);
    }
    catch (MyPLReturnException r)
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
  HeapObject type;
  //  Find head value in symbol_table
  DataObject head_val;
  sym_table.get_val_info(node.first_token().lexeme(), head_val);

  //  Check for nil error
  if (head_val.is_nil())
    error("Cannot assign a null type into another variable ");

  //  Get oid of the type of the node
  size_t type_oid;
  head_val.value(type_oid);
  heap.get_obj(type_oid, type);

  std::list<Token> node_path = node.path;
  node_path.pop_front();

  while (node_path.size() > 1)
  {
    //  Iterate through path and ensure that no values are null
    if(!type.has_att(node_path.front().lexeme()));
      error("Attribute defined as null ");

    node_path.pop_front();
  }

  type.get_val(node.first_token().lexeme(), head_val);
  curr_val = head_val;
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
     curr_val.set(val * -1);
  }
}

#endif
