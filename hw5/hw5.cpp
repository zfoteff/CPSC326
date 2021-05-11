//----------------------------------------------------------------------
// NAME: S. Bowers
// FILE: hw5.cpp
// DATE: Spring 2021
// DESC: HW-5 driver program for testing the type checker.
//----------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include "token.h"
#include "mypl_exception.h"
#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "type_checker.h"

using namespace std;


int main(int argc, char* argv[])
{
  // use standard input if no input file given
  istream* input_stream = &cin;
  if (argc == 2)
    input_stream = new ifstream(argv[1]);

  // create the lexer
  Lexer lexer(*input_stream);
  Parser parser(lexer);
  // read each token in the file until EOS or error
  try {
    Program ast_root_node;
    parser.parse(ast_root_node);
    TypeChecker type_checker;
    ast_root_node.accept(type_checker);
  } catch (MyPLException e) {
    cout << e.to_string() << endl;
    exit(1);
  }
  // clean up the input stream
  if (input_stream != &cin)
    delete input_stream;
}

