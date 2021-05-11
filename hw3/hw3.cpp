//----------------------------------------------------------------------
// NAME: S. Bowers
// FILE: hw3.cpp
// DATE: Spring 2021
// DESC: HW-3 driver program for testing the parser.
//----------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include "token.h"
#include "mypl_exception.h"
#include "lexer.h"
#include "parser.h"


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
    parser.parse();
  } catch (MyPLException e) {
    cout << e.to_string() << endl;
    exit(1);
  }

}

