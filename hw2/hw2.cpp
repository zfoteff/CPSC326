//----------------------------------------------------------------------
// NAME: S. Bowers
// FILE: hw2.cpp
// DATE: Spring 2021
// DESC: HW-2 driver program for testing the lexer.
//----------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include "token.h"
#include "lexer.h"
#include "mypl_exception.h"

using namespace std;

int main(int argc, char* argv[])
{
  // use standard input if no input file given
  istream* input_stream = &cin;
  if (argc == 2)
    input_stream = new ifstream(argv[1]);

  // create the lexer
  Lexer lexer(*input_stream);

  // read each token in the file until EOS or error
  try {
    Token t = lexer.next_token();
    while (t.type() != TokenType::EOS) {
      cout << t.to_string() << endl;
      t = lexer.next_token();
    }
    // print last token read
    cout << t.to_string() << endl;
  } catch (MyPLException e) {
    cout << e.to_string() << endl;
    exit(1);
  }
}

