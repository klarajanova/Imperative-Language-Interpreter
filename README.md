Implementation of the IFJ16 imperative language interpreter.
--------
Authors: Daniel Hladík, Klára Jánová

Assignment:
Create a C program that reads a source file written in IFJ16 and
interprets it. If the interpreter runs without errors, the return value is
0 (zero). If any error occurred, the return value is returned as follows:
- 1 - error in lexical analysis (incorrect structure of the current lexeme).
- 2 - error in the program within the syntactic analysis (incorrect syntax of the program).
- 3 - semantic error in the program - undefined class/function/prominent, attempted redefinition
class/function/variable, etc.
- 4 - semantic type compatibility error in arithmetic, relational and relational
expressions, e.g., wrong concept of parameter type in function calls.
- 6 - other semantic errors.
- 7 - runtime error when calculating a numeric value from the input.
- 8 - runtime error when working with uninitialized variable.
- 9 - runtime error dividing by zero.
- 10 - other runtime errors.
- 99 - internal interpreter error, i.e. not influenced by the input program (e.g. memory allocation error, error when opening a file with a control program, wrong command line parameters, etc.).
The name of the control program file in IFJ16 will be passed as the first and only parameter to the command prompt. It will be possible to specify it with both relative and absolute paths. The program will accept inputs from the standard input, route all its outputs dictated by the control program to the standard output, and route all error messages to the standard error output.

The IFJ16 language is a very simplified subset of Java SE , which is statically typed object-oriented language.
