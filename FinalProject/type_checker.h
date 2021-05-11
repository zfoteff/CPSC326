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
#include "mypl_exception.h"


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
  //  Final project implementations
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
  sym_table.set_vec_info("read", StringVec {"string"});

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
  sym_table.set_vec_info("get", StringVec {"int", "string", "char"});

  //  length
  sym_table.add_name("length");
  sym_table.set_vec_info("length", StringVec {"string", "int"});
}


/* --------------------------------
      Top Level Visitors
---------------------------------*/

void TypeChecker::visit(Program& node)
{
  // push the global environment
  sym_table.push_environment();
  // add built-in functions
  initialize_built_in_types();

  for (Decl* d : node.decls)
    d->accept(*this);

  // check for a main function
  if (sym_table.name_exists("main") and sym_table.has_vec_info("main"))
  {
    StringVec main_info;
    sym_table.get_vec_info("main", main_info);
    std::string return_type = main_info[main_info.size()-1];

    //  Ensure that main function has no parameters
    if (return_type != "int")
      error("Invalid main function: main is of the type 'int', but non-int is returned");
    else if(main_info.size() != 1)
      error("Invalid main function: main function should have no parameters");
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
    error("FunDecl Error| Function declaration matches another function that is already declared in scope ", node.id);

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
      error("FunDecl Error| Unrecognized return type: ", node.return_type);
  }

  StringVec params;
  //  Iterate though params and add them to StringVec and symbol table
  for (FunDecl::FunParam param: node.params)
  {
    sym_table.add_name(param.id.lexeme());
    sym_table.set_str_info(param.id.lexeme(), param.type.lexeme());
    params.push_back(param.type.lexeme());
  }

  params.push_back(node.return_type.lexeme());
  //  Add function name to the environment
  sym_table.add_name(node.id.lexeme());
  sym_table.set_vec_info(node.id.lexeme(), params);

  sym_table.push_environment();\

  //  Add return type
  sym_table.add_name("return");
  sym_table.set_str_info("return", node.return_type.lexeme());

  //  read through all stmts
  for (Stmt* s : node.stmts)
    s->accept(*this);
  sym_table.pop_environment();

  //   If type is nil, then ensure a return stmt does not exist
  if (is_nil)
  {
    if (sym_table.name_exists_in_curr_env("return") && sym_table.has_str_info("return"))
      error("FunDecl Error| Cannot return a value in a nil function");
  }

  //  Otherwise ensure that the type of return stmt
  else
  {
    if (sym_table.name_exists_in_curr_env("return") && sym_table.has_str_info("return"))
    {
      sym_table.get_str_info("return", return_stmt_type);

      // Return error if return types do not match and the return stmt isnt nil
      if ((node.return_type.lexeme() != return_stmt_type) && return_stmt_type != "nil")
        error("FunDecl Error| Return type of function and return stmt do not match, got "
        +node.return_type.lexeme()+" and "+return_stmt_type, node.return_type);
    }
  }
}

void TypeChecker::visit(TypeDecl& node)
{
  //  Check that a type with the same name isn't declared in the scope
  if (sym_table.name_exists_in_curr_env(node.id.lexeme()))
    error("TypeDecl Error| User Defined Type already exists in scope ", node.id);

  StringMap new_type_info;
  sym_table.add_name(node.id.lexeme());
  sym_table.set_map_info(node.id.lexeme(), new_type_info);

  //  Iterate through all declarations and add the to map
  sym_table.push_environment();
  for (VarDeclStmt* vdecl: node.vdecls)
  {
    vdecl->accept(*this);
    new_type_info[vdecl->id.lexeme()] = curr_type;
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
  //  Check that type exists
  if (node.type != nullptr)
    if (sym_table.name_exists(node.type->lexeme()))
      error("VarDeclStmt Error| Type already exists: ", node.id);

  //  If a variable already exists in the env -> shadowing error
  if (sym_table.name_exists_in_curr_env(node.id.lexeme()))
    error("VarDeclStmt Error| Variable shadowing error: ", node.id);

  node.expr->accept(*this);

  //  If type is explicitly declared, and value is nil, assign the type
  if (node.type != nullptr && curr_type == "nil")
    curr_type = node.type->lexeme();

  //  If type is implicitly declared, and value is nil, throw error
  if (node.type == nullptr && curr_type == "nil")
    error("VarDeclStmt Error| Cannot assign nil to a implicitly typed variable: ", node.id);

  //  check if type matches curr_type for explicitly defined vars
  if (node.type != nullptr)
    if (node.type->lexeme() != curr_type && curr_type != "nil")
      error("VarDeclStmt Error| rhs does not match the type of lhs expression: ", node.id);

  //  initialize variable info and add it to the symbol table
  std::string var = node.id.lexeme();
  std::string expr_type = curr_type;
  sym_table.add_name(var);
  sym_table.set_str_info(var, expr_type);
}

// AssignStmt visitor
void TypeChecker::visit(AssignStmt& node)
{
  std::string prev_type;
  std::list<Token> lvals = node.lvalue_list;

  //  Check that the id is defined and get its type
  if (sym_table.name_exists(lvals.front().lexeme()))
    sym_table.get_str_info(lvals.front().lexeme(), curr_type);
  else
    error("AssignStmt Error| Value not found: ", lvals.front());

  prev_type = curr_type;
  lvals.pop_front();
  //  Iterate through the rest of the path (if it exists)
  for (Token t : lvals)
  {
    if (!(sym_table.has_map_info(prev_type)))
      error("AssignStmt Error| Path value doesn't exist: ", t);

    //  Get map info of prev type
    StringMap map;
    sym_table.get_map_info(prev_type, map);

    if (map.count(t.lexeme()) > 0)
      curr_type = map[t.lexeme()];
    else
      error ("AssignStmt Error| Type does not exist: ", t);

    prev_type = curr_type;
  }

  //  Check that lhs matches rhs
  std::string lhs_type = curr_type;
  Expr* e = node.expr;
  e->accept(*this);
  if (lhs_type != curr_type)
    error("AssignStmt Error| lhs type does not match rhs type: ");
}

//  ReturnStmt visitor
void TypeChecker::visit(ReturnStmt& node)
{
  node.expr->accept(*this);
  std::string return_type;

  //  Get return type of function
  if (sym_table.has_str_info("return"))
    sym_table.get_str_info("return", return_type);

  //  check that the type matches the function type
  if (curr_type != return_type && curr_type != "nil")
    error("ReturnStmt Error| Function type does not match returned type", node.expr->first_token());
}

//  IfStmt visitor
void TypeChecker::visit(IfStmt& node)
{
  //  Get type of if stmt condition
  node.if_part->expr->accept(*this);
  //  Ensure if condition evaluates to boolean value
  if (curr_type != "bool")
    error("IfStmt Error| If stmt condition must be a boolean, got "+curr_type+": ", node.if_part->expr->first_token());

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
        error("IfStmt Error| Else if stmt condition must be a boolean, got "+curr_type+": ", s->expr->first_token());

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
    error("WhileStmt Error| While stmt condition must evaluate to bool, got "+curr_type, node.expr->first_token());

  // Push env and typecheck all stmts
  sym_table.push_environment();
  for (Stmt* s : node.stmts)
    s->accept(*this);
  sym_table.pop_environment();
}

//  ForStmt visitor
void TypeChecker::visit(ForStmt& node)
{
  //  Push environment
  sym_table.push_environment();

  //  Get for loop condition type
  node.start->accept(*this);

  //  Add for loop param var to symbol table
  std::string var = node.var_id.lexeme();
  std::string expr_type = curr_type;
  sym_table.add_name(var);
  sym_table.set_str_info(var, expr_type);

  //  Check for proper start conditions
  if (curr_type != "int")
    error("ForStmt Error| Start condition needs to be of type int: ", node.var_id);

  //  End statement
  node.end->accept(*this);

  //  Check for proper end conditions
  if (curr_type != "int")
    error("ForStmt Error| End condition needs to be of type int: ", node.var_id);

  //  Push body environment and iterate through all body statements
  sym_table.push_environment();
  for (Stmt* s : node.stmts)
    s->accept(*this);
  //  Pop body environment
  sym_table.pop_environment();
  //  Pop for environment
  sym_table.pop_environment();
}

void TypeChecker::visit(TryStmt& node)
{
  //  Iterate through all stmts in the try stmt
  sym_table.push_environment();
  for (Stmt* s : node.stmts)
    s->accept(*this);
  sym_table.pop_environment();

  //  typecheck the CatchStmt of the try block
  node.catch_stmt->accept(*this);
}

void TypeChecker::visit(ThrowStmt& node)
{
  //  Typecheck expression and ensure it is a int,double, or bool
  node.expr->accept(*this);
  if (curr_type != "int" && curr_type != "double" && curr_type != "bool")
    error("ThrowStmt Error| Can only throw an int, double, or a bool expression");
}

void TypeChecker::visit(CatchStmt& node)
{
  //  If catch_expr exists evaluate it
  if (node.catch_expr)
  {
    node.catch_expr->accept(*this);
    //  If expression doesn't evaluate to an int double or bool, throw error
    if (curr_type != "int" && curr_type != "double" && curr_type != "bool")
      error("CatchStmt Error| Can only catch an int, double, or bool expression", node.catch_expr->first_token());
  }

  //  Iterate through all stmts in the catch stmt
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
  std::string lhs_type;
  std::string rhs_type;
  //  Get type of lhs and store it
  node.first->accept(*this);
  lhs_type = curr_type;

  if (node.rest)
  {
    node.rest->accept(*this);
    rhs_type = curr_type;
  }

  //  Check that negation rules are properly applied
  if (node.negated && curr_type != "bool")
    error("Expr Error| Cannot negate a non boolean expression: ", node.first_token());

  //  Check operation rules
  if (node.op)
  {
    switch (node.op->type())
    {
      //  Plus operations
      case PLUS:
        // Strings and chars
        if (lhs_type == "char" || lhs_type == "string")
          if (rhs_type == "char" || rhs_type == "string")
            curr_type = "string";
          else
            error ("Expr Error| Can only concatinate strings and chars: ", node.first_token());
        //  Ints and doubles
        else if (lhs_type == "int" && rhs_type == "int")
          curr_type = "int";
        else if (lhs_type == "double" && rhs_type == "double")
          curr_type = "double";
        else
          error ("Expr Error| Can only add ints with ints, and doubles with doubles: ", node.first_token());
        break;
      //  Minus operations
      case MINUS: case MULTIPLY: case DIVIDE:
        if (lhs_type == "int" && rhs_type == "int")
          curr_type = "int";
        else if (lhs_type == "double" && rhs_type == "double")
          curr_type = "double";
        else
          error ("Expr Error| Can only subtract/multiply/divide ints with ints and doubles with doubles: ", node.first_token());
        break;
      //  <, >, <=, >= operations
      case LESS: case GREATER: case LESS_EQUAL: case GREATER_EQUAL:
        if (lhs_type == "int" && rhs_type == "int")
          curr_type = "bool";
        else if (lhs_type == "double" && rhs_type == "double")
          curr_type = "bool";
        else
          error("Expr Error| Can only use comparison operators with int or double values: ", node.first_token());
        break;
      //  % operations
      case MODULO:
        if (lhs_type == "int" && rhs_type == "int")
          curr_type = "int";
        else
          error("Expr Error| Can only use modulus operator with int values: ", node.first_token());
        break;
      //  && and || operations
      case AND: case OR:
        if (lhs_type == "bool" && rhs_type == "bool")
          curr_type = "bool";
        else
          error("Expr Error| Can only use AND and OR operations on boolean values: ", node.first_token());
        break;
      //  Equivalence operations
      case EQUAL: case ASSIGN: case NOT_EQUAL:
        if (lhs_type != rhs_type)
          if (lhs_type != "nil" && rhs_type != "nil")
            error("Expr Error| Equivalence comparisons need to be between matching types or nil values");
        curr_type = "bool";
        break;
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
      error("NewRValue Error| Type that is instantiated has no data associated with it: ", node.type_id);

    //  Assign curr_type to the type of the new instantiated object
    curr_type = node.type_id.lexeme();
  }

  else
    error("Type that is attempting to be declared does not exist: ", node.type_id);
}

//  CallExpr visitor
void TypeChecker::visit(CallExpr& node)
{
  std::string fun_name = node.function_id.lexeme();
  StringVec fun_type;
  sym_table.get_vec_info(fun_name, fun_type);

  //  Check that the function exists
  if (!(sym_table.name_exists(fun_name)))
    error ("CallExpr Error| Function does not exist in the given scope: ", node.function_id);

  //  Check that the proper number of parameters exist
  if (fun_type.size()-1 != node.arg_list.size())
    error("CallExpr Error| Incorrect number of parameters: ", node.function_id);

  int i = 0;
  for (Expr* e : node.arg_list)
  {
    e->accept(*this);
    if (fun_type[i] != curr_type && curr_type != "nil")
      error("CallExpr| Function call does not match function defintion: ", node.function_id);
    i++;
  }

  //  Set curr type to return type
  curr_type = fun_type[fun_type.size()-1];
}

//  IDRValue visitor
void TypeChecker::visit(IDRValue& node)
{
  std::list<Token> path = node.path;
  std::string prev_type;

  //  Check that the first element in path exists
  if (sym_table.name_exists(path.front().lexeme()))
    sym_table.get_str_info(path.front().lexeme(), curr_type);
  else
    error("IDRValue Error| Head element of path does not exist: ", node.path.front());

  prev_type = curr_type;
  //  Remove head element
  path.pop_front();
  //  Iterate through elements in path
  for (Token t : path)
  {
    if (!(sym_table.has_map_info(prev_type)))
      error ("IDRValue Error| Path value does not exist: ", t);

    StringMap map;
    sym_table.get_map_info(prev_type, map);

    //  Check if curr path value exists
    if (map.count(t.lexeme()) > 0)
      curr_type = map[t.lexeme()];
    else
      error("IDRValue Error| Path value does not exist: ", t);

    prev_type = curr_type;
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
}

#endif
