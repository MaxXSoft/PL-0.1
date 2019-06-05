# PL/0.1

Implement a PL/0 compiler/interpreter from scratch.

## EBNF of PL/0.1

```ebnf
program     ::= block ".";
block       ::= [constants] [variables] {procedure | function} statement;
constants   ::= "const" id "=" expression {"," id "=" expression} ";";
variables   ::= "var" id ["=" expression] {"," id ["=" expression]} ";";
procedure   ::= "procedure" id ";" block ";";
function    ::= "function" id ["(" id {"," id} ")"] ";" block ";";
statement   ::= [assign | id | funcall | beginend | if | while | asm | control];
assign      ::= id ":=" expression;
funcall     ::= id "(" expression {"," expression} ")";
beginend    ::= "begin" statement {";" statement} "end";
if          ::= "if" condition "then" statement ["else" statement];
while       ::= "while" condition "do" statement;
asm         ::= "asm" "begin" {anystr [";"]} "end";
control     ::= "break" | "continue";
condition   ::= "odd" expression | expression relation expression;
expression  ::= [addsub] term {addsub term};
term        ::= factor {muldiv factor};
factor      ::= id | number | funcall | "(" expression ")";
id          ::= regex([A-Za-z][A-Za-z0-9]*);
number      ::= regex(0|([1-9][0-9]*)|(\$[0-9a-fA-F]+));
anystr      ::= regex('.+');
relation    ::= "=" | "<>" | "<" | "<=" | ">" | ">=";
addsub      ::= "+" | "-";
muldiv      ::= "*" | "/";
```

## Copyright and License

Copyright (C) 2010-2019 MaxXing. License GPLv3.
