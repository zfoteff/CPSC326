//----------------------------------------------------------------------
// NAME:  Zac Foteff
// FILE:  type_checker.h
// DATE:  March 18 2021
// DESC:  Implementation of a typechecker for MyPL using a visitor-node
//        design scheme
//----------------------------------------------------------------------


#ifndef TYPE_CHECKER_H
#define TYPE_CHECKER_H

#include <iostream>
#include "ast.h"
#include "symbol_table.h"


class TypeChecker : public Visitor
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

private:
  // the symbol table
  SymbolTable sym_table;
  // the previously inferred type
  std::string curr_type;
  // helper to add built in functions
  void initialize_built_in_types();

  // error message
  void error(const std::string& msg, const Token& token);
  void error(const std::string& msg);

};


void TypeChecker::error(const std::string& msg, const Token& token)
{
  throw MyPLException(SEMANTIC, msg, token.line(), token.column());
}


void TypeChecker::error(const std::string& msg)
{
  throw MyPLException(SEMANTIC, msg);
}


void TypeChecker::initialize_built_in_types()
{
  // print function
  sym_table.add_name("print");
  sym_table.set_vec_info("print", StringVec {"string", "nil"});

  //  read
  sym_table.add_name("read");
  sym_table.set_vec_info("read", StringVec {"string", "nil"});

  // stoi
  sym_table.add_name("stoi");
  sym_table.set_vec_info("stoi", StringVec {"string", "int"});

  // itos
  sym_table.add_name("itos");
  sym_table.set_vec_info("itos", StringVec {"int", "string"});

  //  stod
  sym_table.add_name("stod");
  sym_table.set_vec_info("stod", StringVec {"string", "double"});

  //  dtos
  sym_table.add_name("dtos");
  sym_table.set_vec_info("dtos", StringVec {"double", "string"});

  //  get
  sym_table.add_name("get");
  sym_table.set_vec_info("get", StringVec {"string", "char"});

  //  length
  sym_table.add_name("length");
  sym_table.set_vec_info("length", StringVec {"string", "int"});
}


/* --------------------------------
      Top Level Visitors
---------------------------------*/

void TypeChecker::visit(Program& node)
{
  // add built-in functions
  initialize_built_in_types();
  // push the global environment
  sym_table.push_environment();
  // push
  for (Decl* d : node.decls)
    d->accept(*this);
  // check for a main function
  if (sym_table.name_exists("main") and sym_table.has_vec_info("main"))
  {
    StringVec main_info;
    sym_table.get_vec_info("main", main_info);

    //  Ensure that main function has no parameters
    if (main_info.size() > 0)
      error("Main function should have no parameters");
  }
  else
  {
    // NOTE: the only time the 1-argument version of error should be
    // called!
    error("undefined 'main' function");
  }
   // pop the global environment
  sym_table.pop_environment();
}

void TypeChecker::visit(FunDecl& node)
{
  bool is_nil = false;
  std::string return_stmt_type;
  //  Check that function isnt already declared in the current environment
  if (sym_table.name_exists_in_curr_env(node.id.lexeme()))
    error("Function declaration matches another function that is already declared in scope ", node.id);

  //  If return type is nil, raise flag
  if (node.return_type.lexeme() == "nil")
    is_nil = true;

  //  Check that function has a return type that matches primitives, nil, or a UDT
  if (
     node.return_type.lexeme() != "int" && node.return_type.lexeme() != "double"
     && node.return_type.lexeme() != "char" && node.return_type.lexeme() != "string"
     && node.return_type.lexeme() != "bool" && node.return_type.lexeme() != "nil"
     )
  {
    //  Check if name is a UDT that exists in scope
    if ( !(sym_table.name_exists_in_curr_env(node.return_type.lexeme())) )
      error("Unrecognized return type: ", node.return_type);
  }

  StringVec params;
  //  Iterate though params and add them to StringVec
  for (FunDecl::FunParam param: node.params)
    params.push_back(param.type.lexeme());

  //  Add function name to the environment
  sym_table.add_name(node.id.lexeme());
  sym_table.set_vec_info(node.id.lexeme(), params);

  //  push environment and read through all stmts
  sym_table.push_environment();
  for (Stmt* s : node.stmts)
    s->accept(*this);
  sym_table.pop_environment();

  //   If type is nil, then ensure a return stmt does not exist
  if (is_nil)
  {
    if (sym_table.name_exists_in_curr_env("return") && sym_table.has_str_info("return"))
      error("Cannot return a value in a nil function");
  }

  //  Otherwise ensure that the type of return stmt
  else
  {
    if (sym_table.name_exists_in_curr_env("return") && sym_table.has_str_info("return"))
    {
      sym_table.get_str_info("return", return_stmt_type);

      // Return error if return types do not match and the return stmt isnt nil
      if (node.return_type.lexeme() != return_stmt_type && return_stmt_type != "nil")
        error("Return type of function and return stmt do not match, got "
        +node.return_type.lexeme()+" and "+return_stmt_type, node.return_type);
    }
  }
}

void TypeChecker::visit(TypeDecl& node)
{
  //  Check that a type with the same name isn't declared in the scope
  if (sym_table.name_exists_in_curr_env(node.id.lexeme()))
    error("User Defined Type already exists in scope ", node.id);

  StringMap new_type_info;
  sym_table.add_name(node.id.lexeme());

  //  Iterate through all declarations and add the to map
  sym_table.push_environment();
  for (VarDeclStmt* vdecl: node.vdecls)
  {
    vdecl->expr->accept(*this);
    new_type_info[vdecl->type->lexeme()] = curr_type;
  }
  sym_table.pop_environment();

  //  Add UDT info to symbol table
  sym_table.set_map_info(node.id.lexeme(), new_type_info);
}

  /* --------------------------------
        Statement Visitors
  ---------------------------------*/
//  VarDeclStmt visitor
void TypeChecker::visit(VarDeclStmt& node)
{
  //  If a variable already exists in the env -> shadowing error
  if (sym_table.name_exists_in_curr_env(node.id.lexeme()))
    error("Variable shadowing error ", node.id);

  //  If a explicitly declared type exists
  if (node.type != nullptr)
  {
    //  Get type of rhs
    node.expr->accept(*this);

    //  Check that the explicitly defined type matches the type of the rhs expr
    if (node.type->lexeme() != curr_type)
      error("Type error, expected "+node.type->lexeme()+" got "+curr_type, node.id);

    //  Add name and type to the sym_table using the id and the curr_type
    sym_table.add_name(node.id.lexeme());
    sym_table.set_str_info(node.id.lexeme(), curr_type);
  }

  else
  {
    //  Get type of rhs
    node.expr->accept(*this);

    //  Add name and type to the sym_table using the id and the curr_type
    sym_table.add_name(node.id.lexeme());
    sym_table.set_str_info(node.id.lexeme(), curr_type);
  }
}

// AssignStmt visitor
void TypeChecker::visit(AssignStmt& node)
{
  //  Check that lhs variable exists in the environment
  if (sym_table.has_map_info(node.lvalue_list.front().lexeme()))
  {
    std::string lsh_type;
    //  Get lhs type and store it in a variable
    sym_table.get_str_info(node.lvalue_list.front().lexeme(), lsh_type);
    //  Typecheck rhs
    node.expr->accept(*this);

    //  If lsh type dne rhs type or rhs is nil, throw error
    if (lsh_type != curr_type || curr_type == "nil")
      error("Trying to assign "+curr_type+" into a "+lsh_type, node.lvalue_list.front());
  }

  //  If lsh dne in environment throw error
  else
    error ("Trying to assign into uninitialized variable ", node.lvalue_list.front());
}

//  ReturnStmt visitor
void TypeChecker::visit(ReturnStmt& node)
{
  //  If return appears twice in a given environment, error
  if (sym_table.name_exists_in_curr_env("return"))
    error("Return should only be called once ", node.expr->first_token());

  //  Typecheck rhs
  node.expr->accept(*this);
  //  Add return to the symbol table with the type of its expression
  sym_table.add_name("return");
  sym_table.set_str_info("return", curr_type);
}

//  IfStmt visitor
void TypeChecker::visit(IfStmt& node)
{
  //  Get type of if stmt condition
  node.if_part->expr->accept(*this);
  //  Ensure if condition evaluates to boolean value
  if (curr_type != "bool")
    error("If stmt condition must be a boolean, got "+curr_type, node.if_part->expr->first_token());

  // Push env and typecheck all stmts
  sym_table.push_environment();
  for (Stmt* s : node.if_part->stmts)
    s->accept(*this);
  sym_table.pop_environment();

  //  Check to see if else if stmts exist
  if (!(node.else_ifs.empty()))
  {
    for (BasicIf* s : node.else_ifs)
    {
      //  Get type of else if stmt condition
      s->expr->accept(*this);
      //  Ensure condition is a boolean
      if (curr_type != "bool")
        error("Else if stmt condition must be a boolean, got "+curr_type, s->expr->first_token());

      // Push env and typecheck all stmts
      sym_table.push_environment();
      for (Stmt* stmt : s->stmts)
        stmt->accept(*this);
      sym_table.pop_environment();
    }
  }

  //  Check to see if else stmts exist
  if (!(node.body_stmts.empty()))
  {
    // Push env and typecheck all stmts
    sym_table.push_environment();
    for (Stmt* s : node.body_stmts)
      s->accept(*this);
    sym_table.pop_environment();
  }
}

//  WhileStmt visitor
void TypeChecker::visit(WhileStmt& node)
{
  node.expr->accept(*this);

  //  If typechecked expression does not evaluate to a bool
  if (curr_type != "bool")
    error("While stmt condition must evaluate to bool, got "+curr_type, node.expr->first_token());

  // Push env and typecheck all stmts
  sym_table.push_environment();
  for (Stmt* s : node.stmts)
    s->accept(*this);
  sym_table.pop_environment();
}

//  ForStmt visitor
void TypeChecker::visit(ForStmt& node)
{
  std::string start_type;
  //  If the variable id exists in environment, throw an error
  if (sym_table.name_exists_in_curr_env(node.var_id.lexeme()))
    error("Start id already exists", node.var_id);

  // Get type of var id
  node.start->accept(*this);
  // Store that type
  sym_table.add_name(node.var_id.lexeme());
  sym_table.set_str_info(node.var_id.lexeme(), curr_type);
  start_type = curr_type;

  //  Get type of end
  node.end->accept(*this);
  //  Ensure that the end expr type matches type of start expr
  if (curr_type != start_type)
    error("For loop start and end expression types do not match ", node.end->first_token());

  // Push env and typecheck all stmts
  sym_table.push_environment();
  for (Stmt* s : node.stmts)
    s->accept(*this);
  sym_table.pop_environment();
}

/* --------------------------------
        Expression Visitors
---------------------------------*/
//  Expr visitor
void TypeChecker::visit(Expr& node)
{
  //  Two main cases for handling these expressions
  //  1. Operator doesn't exist
  if (node.op == nullptr)
  {
    //  Get type of expr
    node.first->accept(*this);

    //  If the expr is negated ensure that the expression is a boolean
    if (node.negated && curr_type != "bool")
      error("Expected not keyword to be followed by boolean expression, got "+curr_type, node.first_token());
  }

  //  2. Operator does exist
  else
  {
    std::string lhs_type;

    //  Typecheck lhs and store it in a variable
    node.first->accept(*this);
    lhs_type = curr_type;

    if (node.rest != nullptr)
    {
      //  Typecheck rhs
      node.rest->accept(*this);

      //  Cases for operand
      switch (node.op->type())
      {
        //  Equivalence operators
        case EQUAL: case NOT_EQUAL:
        {
          //  Ensure that both types are int, double, bool, string, char, nil, or UDT
          if (lhs_type != curr_type)
          {
            //  If one of the types is nil and the other isnt, still a valid expr
            if ( (lhs_type == "nil" && curr_type != "nil") || (lhs_type != "nil" && curr_type == "nil"))
              curr_type = "bool";

            else
              error("Cannot compare two different types ", node.first_token());
          }

          //  Report type of expr as boolean
          curr_type = "bool";
        }

        //  Comparision operators
        case GREATER: case GREATER_EQUAL: case LESS: case LESS_EQUAL:
        {
          //  If the type types are not equal, then they cannot be compared
          if (lhs_type != curr_type)
            error("Cannot compare "+lhs_type+" and "+curr_type, node.first_token());

          //  If the types of lhs and rhs are not int, double, char, or str values
          if
          ( (lhs_type != "int" || lhs_type != "double" || lhs_type != "char" || lhs_type != "string")
            && (curr_type != "int" || curr_type != "double" || curr_type != "char" || curr_type != "string")
          )
            error("Can only compare primitive values, got "+lhs_type, node.first_token());

          curr_type = "bool";
        }

        //  Logical operators
        case AND: case OR: case NOT:
        {
          //  If both types are not equal to bool, error
          if (lhs_type != "bool" && curr_type != "bool")
            error ("Can only perform logical operations on boolean values, got "+lhs_type+" and "+curr_type, node.first_token());

          curr_type = "bool";
        }

        //  Arithmetic operators
        case PLUS:
        {
          //  Check that types are either double and/or int, or char and/or string
          //  int and double
          if (lhs_type == "int" || lhs_type == "double")
          {
            if (lhs_type == "int" && curr_type == "int")
              curr_type = "int";

            else if (lhs_type == "double" && curr_type == "double")
              curr_type = "double";

            else
            //  trying to add types other than int or double
              error("Can only add like types, got "+lhs_type+" and "+curr_type, node.first_token());
          }

          //  char and str
          if (lhs_type == "char" || lhs_type == "string")
          {
            if (curr_type == "char" || curr_type == "string")
              curr_type == "string";

            else
              error("Can only add char to string or vice versa, got "+lhs_type+" and "+curr_type, node.first_token());
          }

          else
            error("Cannot add "+lhs_type+" and "+curr_type, node.first_token());
        }

        case MINUS: case MULTIPLY: case DIVIDE:
        {
          //  Check that types are either double and/or int
          //  int and double
          if (lhs_type == "int" || lhs_type == "double")
          {
            if (lhs_type == "int" && curr_type == "int")
              curr_type = "int";

            else if (lhs_type == "double" && curr_type == "double")
              curr_type = "double";

            else
            //  trying to operate on types other than int or double
              error("Can only operate on like types, got "+lhs_type+" and "+curr_type, node.first_token());
          }

          else
            error("Can't perform Arithmetic operations on non int or double types", node.first_token());
        }

        case MODULO:
        {
          //  Check that both types are ints
          if ( !(lhs_type == "int" && curr_type == "int") )
            error("Can only find modulus of integers ", node.first_token());
        }
      }
    }
  }
}

//  SimpleTerm visitor
void TypeChecker::visit(SimpleTerm& node)
{
  //  Typecheck the expression
  node.rvalue->accept(*this);
}

//  ComplexTerm visitor
void TypeChecker::visit(ComplexTerm& node)
{
  //  typecheck the expression
  node.expr->accept(*this);
}

/* --------------------------------
      Rvalue Visitors
---------------------------------*/
//  SimpleRValue visitor
void TypeChecker::visit(SimpleRValue& node)
{
  switch (node.value.type())
  {
    case INT_VAL:
      curr_type = "int";
      break;

    case CHAR_VAL:
      curr_type = "char";
      break;

    case BOOL_VAL:
      curr_type = "bool";
      break;

    case DOUBLE_VAL:
      curr_type = "double";
      break;

    case STRING_VAL:
      curr_type = "string";
      break;

    case NIL:
      curr_type = "nil";
      break;

    default:
      error("Unexpected Value ");
      break;
  }
}

//  NewRValue visitor
void TypeChecker::visit(NewRValue& node)
{
  //  Ensure that new type exists in environment
  if (sym_table.name_exists_in_curr_env(node.type_id.lexeme()))
  {
    //  Ensure that the type has data associated with it
    if ( !(sym_table.has_map_info(node.type_id.lexeme())) )
      error("Type that is instantiated has no data associated with it ", node.type_id);

    //  Assign curr_type to the type of the new instantiated object
    curr_type = node.type_id.lexeme();
  }

  else
    error("Type that is attempting to be declared does not exist: ", node.type_id);
}

//  CallExpr visitor
void TypeChecker::visit(CallExpr& node)
{
  StringVec func_args;

  //  Ensure that the function exists in the environment
  cout<< node.function_id.lexeme();
  if ( !(sym_table.name_exists_in_curr_env(node.function_id.lexeme())) )
    error("Function does not exist in current scope: ", node.function_id);

  sym_table.get_vec_info(node.function_id.lexeme(), func_args);

  //  If there are a different amount of arguments in the node arg list and the
  //  called expression, declaration error
  if (func_args.size() != node.arg_list.size())
    error("Amount of arguments in function call differs from declaration", node.function_id);

  for (int i = 0; i < func_args.size(); i++)
  {
    //  If one of the entered parameters either argument doesn't match, declaration error
    if (func_args[i] != node.arg_list.front()->first_token().lexeme())
      error("Expected a "+ node.arg_list.front()->first_token().lexeme() + ", got "+ func_args[i], node.function_id);

    node.arg_list.pop_front();
  }
}

//  IDRValue visitor
void TypeChecker::visit(IDRValue& node)
{
  //  Check that the variable exists in scope
  if ( !(sym_table.name_exists_in_curr_env(node.first_token().lexeme())) )
    error("Start of path expression does not exist in scope ", node.first_token());

  // Get UDT type info
  std::string UDT_type_name;
  StringMap UDT_type_info;
  sym_table.get_str_info(node.first_token().lexeme(), UDT_type_name);

  //  If UDT does not exist that is trying to be made a path of
  if (!(sym_table.has_map_info(UDT_type_name)))
    error("User declared type exists, but has not been defined ", node.first_token());

  sym_table.get_map_info(UDT_type_name, UDT_type_info);

  // Copy the arguments for easy iteration and remove first element
  std::list<Token> args;
  Token prev_token;
  prev_token = node.path.front();
  args = node.path;
  args.pop_front();

  //  Check that the path expression exists and the type contains data
  while (!(args.empty()))
  {
    //  Check that this arg is a member variable of the type & set the curr type as it
    if (UDT_type_info.count(args.front().lexeme()) > 0)
    {
      std::string type = UDT_type_info[args.front().lexeme()];
      curr_type = type;
    }
  }
}

//  NegatedRValue visitor
void TypeChecker::visit(NegatedRValue& node)
{
  //  Get type of expr
  node.expr->accept(*this);
  //  If the type of the expression isnt a int or double throw an error
  if (curr_type != "int" || curr_type != "double")
    error("Cannot negate "+curr_type+". Expected int or double", node.expr->first_token());

  //  Get type of expression to be negated
  node.expr->accept(*this);
}

#endif
