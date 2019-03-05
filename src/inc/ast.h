#ifndef PL01_AST_H_
#define PL01_AST_H_

#include <memory>
#include <utility>
#include <string>
#include <vector>
#include <map>

#include <lexer.h>

class BaseAST {
public:
    virtual ~BaseAST() = default;
};

using ASTPtr = std::unique_ptr<BaseAST>;
using ASTPtrList = std::vector<ASTPtr>;
using VarDef = std::pair<std::string, ASTPtr>;
using VarDefList = std::vector<VarDef>;
using IdList = std::vector<std::string>;

class BlockAST : public BaseAST {
public:
    BlockAST(ASTPtr consts, ASTPtr vars, ASTPtrList proc_func, ASTPtr stat)
            : consts_(std::move(consts)), vars_(std::move(vars)),
              stat_(std::move(stat)), proc_func_(std::move(proc_func)) {}

private:
    ASTPtr consts_, vars_, stat_;
    ASTPtrList proc_func_;
};

class ConstsAST : public BaseAST {
public:
    ConstsAST(VarDefList defs) : defs_(std::move(defs)) {}

private:
    VarDefList defs_;
};

class VarsAST : public BaseAST {
public:
    VarsAST(VarDefList defs) : defs_(std::move(defs)) {}

private:
    VarDefList defs_;
};

class ProcedureAST : public BaseAST {
public:
    ProcedureAST(const std::string &id, ASTPtr block)
            : id_(id), block_(std::move(block)) {}

private:
    std::string id_;
    ASTPtr block_;
};

class FunctionAST : public BaseAST {
public:
    FunctionAST(const std::string &id, IdList args, ASTPtr block)
            : id_(id), args_(std::move(args)),
              block_(std::move(block)) {}

private:
    std::string id_;
    IdList args_;
    ASTPtr block_;
};

class AssignAST : public BaseAST {
public:
    AssignAST(const std::string &id, ASTPtr expr)
            : id_(id), expr_(std::move(expr)) {}

private:
    std::string id_;
    ASTPtr expr_;
};

class CallAST : public BaseAST {
public:
    CallAST(const std::string &id) : id_(id) {}

private:
    std::string id_;
};

class BeginEndAST: public BaseAST {
public:
    BeginEndAST(ASTPtrList stats) : stats_(std::move(stats)) {}

private:
    ASTPtrList stats_;
};

class IfAST : public BaseAST {
public:
    IfAST(ASTPtr cond, ASTPtr then, ASTPtr else_then)
            : cond_(std::move(cond)), then_(std::move(then)),
              else_then_(std::move(else_then)) {}

private:
    ASTPtr cond_, then_, else_then_;
};

class WhileAST : public BaseAST {
public:
    WhileAST(ASTPtr cond, ASTPtr body)
            : cond_(std::move(cond)), body_(std::move(body)) {}

private:
    ASTPtr cond_, body_;
};

class AsmAST : public BaseAST {
public:
    AsmAST(const std::string &asm_str) : asm_str_(asm_str) {}

private:
    std::string asm_str_;
};

class ControlAST : public BaseAST {
public:
    ControlAST(Lexer::Keyword type) : type_(type) {}

private:
    Lexer::Keyword type_;
};

class UnaryAST : public BaseAST {
public:
    UnaryAST(Lexer::Keyword op, ASTPtr operand)
            : op_(op), operand_(std::move(operand)) {}

private:
    Lexer::Keyword op_;     // 'odd' only
    ASTPtr operand_;
};

class BinaryAST : public BaseAST {
public:
    BinaryAST(Lexer::Operator op, ASTPtr lhs, ASTPtr rhs)
            : op_(op), lhs_(std::move(lhs)), rhs_(std::move(rhs)) {}

private:
    Lexer::Operator op_;
    ASTPtr lhs_, rhs_;
};

class FunCallAST : public BaseAST {
public:
    FunCallAST(const std::string &id, ASTPtrList args)
            : id_(id), args_(std::move(args)) {}

private:
    std::string id_;
    ASTPtrList args_;
};

class IdAST : public BaseAST {
public:
    IdAST(const std::string &id) : id_(id) {}

private:
    std::string id_;
};

class NumberAST : public BaseAST {
public:
    NumberAST(int value) : value_(value) {}

private:
    int value_;
};

#endif // PL01_AST_H_
