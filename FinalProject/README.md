Exception Handling with Try-Catch blocks
========================================

Try-Catch blocks allow programmers to control the flow of execution in their programs, as well as manage possible exception cases.
For this implementation of MyPL, users are given the capability of checking for two major errors (divide by zero and index out of bounds)
as well as the ability to raise expressions to be caught by the Try-Catch block. The construct is implemented into each different component of
the MyPL language

### To build
```
make
```

### To run
```
./mypl tests/<test_selection>
```

##  Tokens
Three new tokens were added to the MyPL language
* TRY: For indicating the start of the Try-Catch block
* THROW: For raising expressions to be handled by the catch expression. Optional
* CATCH: For catching errors and expressions. Must be included in Try-Catch block

##  Errors
Two new errors were also added to the mypl_exception.h file. The first was the
ZERODIVISION error and a INDEXOUTOFBOUNDS error. These will be checked for in the interpreter
when implementing the blocks. Additionally, a get_type() function was implemented to be able to see the type of a throw error

## Lexer
The Lexer was modified to give it the capability to recognize TRY, CATCH, and THROW tokens.
Since these are reserved words, it was straightforward to construct tokens to Try-Catch blocks

## AST
The AST file was modified to add definitions for TryStmt, CatchStmt, and ThrowStmt nodes.
TryStmt nodes contain a null pointer to a CatchStmt and a list of statement node pointers that make up
the body of the statement. CatchStmt nodes contain a Token that stores the ID of an error (if one exists),
a pointer to an Expr node for catching thrown expressions (if one exists). Finally, ThrowStmt nodes only contain
an expression that will be thrown the the catch statement. Each node was also given visitor access and cleanup methods.

## Parser
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

##  TypeChecker
The type-checking rules for Try-Catch blocks mainly pertain to catch and throw statements. For TryStmts, simply iterate through all
statements in the node body, then call the type checker on its CatchStmt node. For the CatchStmt, if the statement is supposed to catch an
expression, then ensure the expression is an int, bool, or double. For the ThrowStmt, the expression should evaluate to an int, bool, or double.

##  Interpreter
Try-Catch blocks are implemented mainly in the TryStmt visitor, as most comparisons between values are available in the TryStmt node. First, if the
CatchStmt node in the TryStmt node is non-null, then get the value of the catch expression and store it for later comparison. Next, push a temporary environment while
iterating through all statements in the TryStmt body. Using a try statement to check, if a expression is triggered by a program, the interpreter checks if the CatchStmt ID
is equal to the type of error thrown. If they are the same, the try statement should stop evaluating its body, pop the environment, and allow it's CatchStmt to handle the
error case. Otherwise, the interpreter should examine curr_value to ensure that the expression to be caught is never thrown. If the expression is reached in the Try body,
then the program should indicate that the value has been reached and the interpreter should stop evaluating body statements in the Try body and transfer evaluation control to
it's CatchStmt.  
The CatchStmt is implemented by creating a new environment, then evaluating all the statements that exist within the CatchStmt body. ThrowStmts simply have their expressions
evaluated and put into the curr_value when called by a visitor.
