CPSC 326: Organization of Programming Languages
===============================================
All files and projects completed during Spring 2021 semester of CPSC326 at
Gonzaga University. Each file should have a make file, or included instructions on how to run them.

## Project 1: Tokens
This first project simply asked us to implement the Token class in C++
which includes types and Lexemes that will be used in our implementation of MyPL.

## Project 2: Lexer
Project 2 implements a lexer that generates tokens from a source mypl file. The
tokens are built from keywords and ids in the source file, and the line and column
number of each token for error case handling.

## Project 3: Parser
Project 3 implements a recursive decent parser for performing semantic analysis
on MyPL source files. The parser ensures that proper MyPL grammer is followed, with
these grammar rules being defined in the hw-3 pdf.

## Project 4: AST Generation
Project 4 called for modification to the recursive decent parser in order to generate
the MyPL abstract syntax tree as well as implement a printer file that outputs the
created AST to the command line to ensure it is build correctly. The parser had to
be modified in order to implement the visitor design pattern in order to build up the
nodes in the AST with the relevant information included in the token stream.

## Project 5: Type Checker
Project 5 called for the implementation of a type checker for MyPL using the visitor
design paradigm. Each node in the AST is checker to ensure that each operation in a MyPL
source file is well typed and environments are defined for specific scopes

## Project 6: Interpreter
Project 6 pulls all of pieces for MyPL together an implements MyPL file into an executable
Interpreter for MyPL. The Interpreter uses a similar visitor design scheme as the type checker and parser. Each node starting at the root program node is visited and each expression in the collection of definitions and statements in a MyPL file. The files are 'executed' using C++ to evaluate each node and its collection of values.

## Project 7: Ocaml Intro
Project 7 had us implement multiple different functions using the logical programming language OCaml. This assignment was intended to introduce us to functional programming language paradigms.

## Project 8: Ocaml Conditionals: If-then statements v. Pattern Matching
Project 8 was a two part assignment. The first part of the assignment implements 10 different functions that operate on lists in Ocaml using conditional statements. The second part of the assignment implements those same 10 functions using pattern matching instead of conditional statement. Each function was tested using a basic unit test system.

## Project 9: Ocaml Algebraic Types
Project 9 acted as an introduction to Algebraic types in Ocaml. This assignment had us define a basic key-value Algebraic type in Ocaml and then define 10 basic functions for operating on that the algebraic type. Each function is tested using a basic unit test system.

## Project 10: Prolog intro
Project 10 was a three part introduction into all different aspects of the Prolog logical programming language. The goal of this project was to gain a firm understanding of logical programming paradigms.

## Final Project:
For our final project I implemented. Try-Catch blocks allow programmers to control the flow of execution in their programs, as well as manage possible exception cases.
For this implementation of MyPL, users are given the capability of checking for two major errors (divide by zero and index out of bounds)
as well as the ability to raise expressions to be caught by the Try-Catch block. The construct is implemented into each different component of
the MyPL language

#### To build
```
make
```

#### To run
```
./mypl tests/<test_selection>
```

###  Tokens
Three new tokens were added to the MyPL language
* TRY: For indicating the start of the Try-Catch block
* THROW: For raising expressions to be handled by the catch expression. Optional
* CATCH: For catching errors and expressions. Must be included in Try-Catch block

###  Errors
Two new errors were also added to the mypl_exception.h file. The first was the
ZERODIVISION error and a INDEXOUTOFBOUNDS error. These will be checked for in the interpreter
when implementing the blocks. Additionally, a get_type() function was implemented to be able to see the type of a throw error

### Lexer
The Lexer was modified to give it the capability to recognize TRY, CATCH, and THROW tokens.
Since these are reserved words, it was straightforward to construct tokens to Try-Catch blocks

### AST
The AST file was modified to add definitions for TryStmt, CatchStmt, and ThrowStmt nodes.
TryStmt nodes contain a null pointer to a CatchStmt and a list of statement node pointers that make up
the body of the statement. CatchStmt nodes contain a Token that stores the ID of an error (if one exists),
a pointer to an Expr node for catching thrown expressions (if one exists). Finally, ThrowStmt nodes only contain
an expression that will be thrown the the catch statement. Each node was also given visitor access and cleanup methods.

### Parser
The parser was modified to have the capability to perform semantic analysis on the components of a Try-Catch block and built AST
nodes for the elements.  
Try
* Ensures the 'try' token exists
* Iterates through all statements in the body of the Try block
  * Once a catch token is found, stop
* Create a new CatchStmt and parse it
* Add a pointer to the CatchStmt to the new TryStmt node

Catch
* Ensure the 'catch' token exists
* Ensure either an expression, or an ID exists between parenthesis followed by the 'then' token
* Evalute all statements in the catch block until the 'end' token is found

Throw
* Ensure the 'throw' token exists
* Parse the expression that follows

###  TypeChecker
The type-checking rules for Try-Catch blocks mainly pertain to catch and throw statements. For TryStmts, simply iterate through all
statements in the node body, then call the type checker on its CatchStmt node. For the CatchStmt, if the statement is supposed to catch an
expression, then ensure the expression is an int, bool, or double. For the ThrowStmt, the expression should evaluate to an int, bool, or double.

###  Interpreter
Try-Catch blocks are implemented mainly in the TryStmt visitor, as most comparisons between values are available in the TryStmt node. First, if the
CatchStmt node in the TryStmt node is non-null, then get the value of the catch expression and store it for later comparison. Next, push a temporary environment while
iterating through all statements in the TryStmt body. Using a try statement to check, if a expression is triggered by a program, the interpreter checks if the CatchStmt ID
is equal to the type of error thrown. If they are the same, the try statement should stop evaluating its body, pop the environment, and allow it's CatchStmt to handle the
error case. Otherwise, the interpreter should examine curr_value to ensure that the expression to be caught is never thrown. If the expression is reached in the Try body,
then the program should indicate that the value has been reached and the interpreter should stop evaluating body statements in the Try body and transfer evaluation control to
it's CatchStmt.  
The CatchStmt is implemented by creating a new environment, then evaluating all the statements that exist within the CatchStmt body. ThrowStmts simply have their expressions
evaluated and put into the curr_value when called by a visitor.
