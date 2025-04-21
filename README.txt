tlisp - A Tiny Lisp Interpreter in C
====================================

Overview
--------
tlisp is a small Lisp interpreter written in C. It supports basic arithmetic operations,
variable definitions, conditionals, and built-in math functions.

This is just a project to see if I could implement something (educational purposes).

Features
--------
- Primitive lisp operations: atom, quote, eq, car, cdr, cons, cond
- Basic arithmetic: +, -, *, /
- Math functions: abs, sqrt, floor, ceil, max, min, mod (%)
- Variable definitions: (define x 10)
- Conditional expressions: (if cond then-expr else-expr)
- Boolean literals: #t and #f

Building
--------
To build the interpreter:

    make

This will produce a binary in `./bin/tlisp`.

Usage
-----
Run the interpreter:

    ./bin/tlisp

Example session:

    > (define x 10)
    10
    > (* x 2)
    20
    > (if #t 1 2)
    1
    > (mod 10 3)
    1
    > (quote a)
    a
    > (cons (quote a) (quote (b c)))
    (a b c)
    > (car (cons (quote a) (quote (b c))))
    (b c)

Project Structure
-----------------
- `src/` — Source code files
- `include/` — Header files
- `bin/` — Compiled binary
- `Makefile` — Build instructions

Notes
-----
- Only numerical types are currently supported.
- Division by zero will result in an error and exit the program.
- All symbols are stored in an environment hash table with support for lexical scoping.