#ifndef PL01_FRONT_PARSER_H_
#define PL01_FRONT_PARSER_H_

/*

EBNF of PL/0.1

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

*/

#include <front/lexer.h>
#include <define/ast.h>

class Parser {
public:
    Parser(Lexer &lexer) : lexer_(lexer), error_num_(0) {
        NextToken();
    }

    ASTPtr ParseProgram();
    void Reset() {
        lexer_.Reset();
        error_num_ = 0;
        NextToken();
    }

    unsigned int error_num() const { return error_num_; }

private:
    using Token = Lexer::Token;
    using Keyword = Lexer::Keyword;
    using Operator = Lexer::Operator;

    Token NextToken() { return cur_token_ = lexer_.NextToken(); }
    bool IsTokenChar(char c) const {
        return cur_token_ == Token::Char && lexer_.char_val() == c;
    }
    bool IsTokenKeyword(Keyword key) const {
        return cur_token_ == Token::Keyword && lexer_.key_val() == key;
    }
    bool IsTokenOperator(Operator op) const {
        return cur_token_ == Token::Operator && lexer_.op_val() == op;
    }
    bool IsRelationalOp() const {
        return cur_token_ == Token::Operator
                && (lexer_.op_val() == Operator::Less
                || lexer_.op_val() == Operator::LessEqual
                || lexer_.op_val() == Operator::Great
                || lexer_.op_val() == Operator::GreatEqual
                || lexer_.op_val() == Operator::NotEqual
                || lexer_.op_val() == Operator::Equal);
    }
    bool IsAddSub() const {
        return cur_token_ == Token::Operator
                && (lexer_.op_val() == Operator::Add
                || lexer_.op_val() == Operator::Sub);
    }
    bool IsMulDiv() const {
        return cur_token_ == Token::Operator
                && (lexer_.op_val() == Operator::Mul
                || lexer_.op_val() == Operator::Div);
    }

    ASTPtr PrintError(const char *message);
    ASTPtr ParseBlock();
    ASTPtr ParseConstants();
    ASTPtr ParseVariables();
    ASTPtr ParseProcedure();
    ASTPtr ParseFunction();
    ASTPtr ParseStatement();
    ASTPtr ParseIdStat();
    ASTPtr ParseFunCall(const std::string &id);
    ASTPtr ParseBeginEnd();
    ASTPtr ParseIf();
    ASTPtr ParseWhile();
    ASTPtr ParseAsm();
    ASTPtr ParseControl();
    ASTPtr ParseCondition();
    ASTPtr ParseExpression();
    ASTPtr ParseTerm();
    ASTPtr ParseFactor();

    Lexer &lexer_;
    unsigned int error_num_;
    Token cur_token_;
};

#endif // PL01_FRONT_PARSER_H_
