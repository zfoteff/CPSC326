//----------------------------------------------------------------------
// NAME:  Zac Foteff
// FILE:  printer.h
// DATE:  02/23/21
// DESC:  Implementation of a visitor design indended to print out an
//        AST using MyPL code styling
//----------------------------------------------------------------------


#ifndef PRINTER_H
#define PRINTER_H

#include <iostream>
#include "ast.h"
using namespace std;

class Printer : public Visitor
{
public:
  // constructor
  Printer(std::ostream& output_stream) : out(output_stream) {}

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
  void visit(TryStmt& node);
  void visit(CatchStmt& node);
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
  std::ostream& out;
  int indent = 0;

  void inc_indent() {indent += 3;}
  void dec_indent() {indent -= 3;}
  std::string get_indent() {return std::string(indent, ' ');}

};


//  Top level implementation
void Printer::visit(Program& node)
{
  for (Decl* d: node.decls)
    d->accept(*this);
}

//  Accept func decl visitor
void Printer::visit(FunDecl& node)
{
  cout << "fun ";
  cout << node.return_type.lexeme() << " " << node.id.lexeme() << "(";
  int num_params = node.params.size();

  for (FunDecl::FunParam f : node.params)
  {
    cout << f.id.lexeme() << ":" << f.type.lexeme();

    if (num_params != 1)
    //  While there are still params in the list, print a comma and repeat loop
      cout << ", ";
    num_params--;
  }

  cout << ")\n";
  inc_indent();
  for (Stmt* s: node.stmts)
  {
    //  Read and print all stmts in the func decl
    cout << get_indent();
    s->accept(*this);
    cout << endl;
  }

  dec_indent();
  cout << "end\n" << endl;
}

//  Accept type decl visitor
void Printer::visit(TypeDecl& node)
{
  cout << "type " << node.id.lexeme() << "\n";
  inc_indent();
  for (VarDeclStmt* v : node.vdecls)
  {
    //  Read and print all stmts in the type decl
    cout << get_indent();
    v->accept(*this);
    cout << endl;
  }

  dec_indent();
  cout << "end\n" << endl;
}

//  Accept var decl visitor
void Printer::visit(VarDeclStmt& node)
{
  cout << "var " << node.id.lexeme();

  if (node.type != nullptr)
    cout << ":" << node.type->lexeme();

  cout << " = ";
  node.expr->accept(*this);
}

//  Accept assign stmt visitor
void Printer::visit(AssignStmt& node)
{
  //  If there is only one value, print it then pop it from the list
  if (node.lvalue_list.size() == 1)
  {
    cout << node.lvalue_list.front().lexeme();
    node.lvalue_list.pop_front();
  }
  else
  {
    //  Print, then delete the first element of the list to print dots out right
    cout << node.lvalue_list.front().lexeme();
    node.lvalue_list.pop_front();
    for (Token t : node.lvalue_list)
      cout << "." << t.lexeme();
  }

  cout << " = ";
  node.expr->accept(*this);
}

//  Accept return stmt visitor
void Printer::visit(ReturnStmt& node)
{
  cout << "return ";
  node.expr->accept(*this);
}

//  Accept if stmt visitor
void Printer::visit(IfStmt& node)
{
  //  Print out ifstmt
  cout << "if ";
  node.if_part->expr->accept(*this);
  cout << " then" << endl;
  inc_indent();

  for (Stmt* s: node.if_part->stmts)
  {
    //  Read through all if stmts in the if part of the stmt
    cout << get_indent();
    s->accept(*this);
    cout << endl;
  }

  if (node.else_ifs.size() != 0)
  {
    //  If else if's exist then read through and print them
    for (BasicIf* bi: node.else_ifs)
    {
      //  Read each of the basic if components of the elseifs
      dec_indent();
      cout << get_indent() << "elseif ";
      bi->expr->accept(*this);
      cout << " then" << endl;
      inc_indent();

      for (Stmt* s: bi->stmts)
      {
        cout << get_indent();
        s->accept(*this);
        cout << endl;
      }
    }
  }

  if (node.body_stmts.size() != 0)
  {
    //  If else stmts exit read through and print them
    dec_indent();
    cout << get_indent() << "else" << endl;
    inc_indent();

    for (Stmt* s: node.body_stmts)
    {
      cout << get_indent();
      s->accept(*this);
      cout << endl;
    }
  }

  dec_indent();
  cout << get_indent() << "end";
}

//  Accept while stmt visitor
void Printer::visit(WhileStmt& node)
{
  //  Print out while stmt
  cout << "while ";
  node.expr->accept(*this);
  cout << " do" << endl;
  inc_indent();

  for (Stmt* s: node.stmts)
  {
    //  Print out all of the stmts in the while loop
    cout << get_indent();
    s->accept(*this);
    cout << endl;
  }

  dec_indent();
  cout << get_indent() << "end";
}

//  Accept for stmt visitor
void Printer::visit(ForStmt& node)
{
  //  Print out the ForStmt
  cout << "for " << node.var_id.lexeme();
  node.start->accept(*this);
  cout << " to ";
  node.end->accept(*this);
  cout << " do" << endl;
  inc_indent();

  for (Stmt* s : node.stmts)
  {
    //  Print out all Stmts in the for loop
    cout << get_indent();
    s->accept(*this);
    cout << endl;
  }

  dec_indent();
  cout << get_indent() << "end";
}

//  Final project visitors

void Printer::visit(TryStmt& node)
{
  cout << "try "<<endl;
  inc_indent();
  for (Stmt* s : node.stmts)
  {
    cout<<get_indent();
    s->accept(*this);
    cout<<endl;
  }

  dec_indent();
}

void Printer::visit(CatchStmt& node)
{
  cout << get_indent() << "catch "<<endl;
  cout << "(";
  if (node.catch_expr == nullptr)
    cout<< node.error_id.lexeme();
  else
    node.catch_expr->accept(*this);

  cout<< ")"<<endl;
  inc_indent();
  for (Stmt* s : node.stmts)
  {
    cout<<get_indent();
    s->accept(*this);
    cout<<endl;
  }

  dec_indent();
  cout << get_indent() << "end";
}

// expressions
void Printer::visit(Expr& node)
{
  //  Handle printing all expression in the program
  if (node.op != nullptr)
  {
    //  Print opening LPAREN
    cout<< "(";
    if (node.negated)
      cout << "neg ";

    node.first->accept(*this);
    cout << " "+node.op->lexeme()+" ";
    node.rest->accept(*this);
    //  Print closing paren
    cout << ")";
  }

  else
  {
    //  print simple stmt
    if (node.negated)
      cout << "neg ";

    node.first->accept(*this);
  }
}

//  Accept simple term visitor
void Printer::visit(SimpleTerm& node) { node.rvalue->accept(*this); }

//  Accept complex term visitor
void Printer::visit(ComplexTerm& node) { node.expr->accept(*this); }

// rvalues
void Printer::visit(SimpleRValue& node) { cout << node.value.lexeme(); }

//  Accept new rval term visitor
void Printer::visit(NewRValue& node) { cout << "new " << node.type_id.lexeme(); }

//  Accept call expr visitor
void Printer::visit(CallExpr& node)
{
  cout << node.function_id.lexeme() << "(";
  if (node.arg_list.size() == 1)
    node.arg_list.front()->accept(*this);

  else if (node.arg_list.size() > 1)
  {
    //  Output first arg, then remove it and read through the rest of the args
    node.arg_list.front()->accept(*this);
    node.arg_list.pop_front();
    for (Expr* e : node.arg_list)
    {
      cout << ", ";
      e->accept(*this);
    }
  }

  cout << ")";
}

//  Accept idrval visitor
void Printer::visit(IDRValue& node)
{
  if (node.path.size() == 1)
    cout << node.path.front().lexeme();
  else
  {
    cout << node.path.front().lexeme();
    node.path.pop_front();

    for (Token t : node.path)
      cout << "." << t.lexeme();
  }
}

//  Accept negated rval visitor
void Printer::visit(NegatedRValue& node) { node.expr->accept(*this); }

#endif
