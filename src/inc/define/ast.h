#ifndef PL01_DEFINE_AST_H_
#define PL01_DEFINE_AST_H_

#include <memory>
#include <utility>
#include <string>
#include <vector>
#include <map>

#include <front/lexer.h>
#include <front/analyzer.h>
#include <define/symbol.h>

class BaseAST {
public:
    virtual ~BaseAST() = default;

    virtual SymbolType SemaAnalyze(Analyzer &ana) = 0;

    unsigned int line_pos() const { return line_pos_; }
    const EnvPtr &env() const { return env_; }

protected:
    void set_line_pos(unsigned int line_pos) { line_pos_ = line_pos; }
    void set_env(const EnvPtr &env) { env_ = env; }

private:
    unsigned int line_pos_;
    EnvPtr env_;
};

using ASTPtr = std::unique_ptr<BaseAST>;
using ASTPtrList = std::vector<ASTPtr>;
using VarDef = std::pair<std::string, ASTPtr>;
using VarDefList = std::vector<VarDef>;
using IdList = std::vector<std::string>;

class BlockAST : public BaseAST {
public:
    BlockAST(ASTPtr consts, ASTPtr vars, ASTPtrList proc_func,
            ASTPtr stat, unsigned int line_pos)
            : consts_(std::move(consts)), vars_(std::move(vars)),
              stat_(std::move(stat)), proc_func_(std::move(proc_func)) {
        set_line_pos(line_pos);
    }

    SymbolType SemaAnalyze(Analyzer &ana) override;

private:
    ASTPtr consts_, vars_, stat_;
    ASTPtrList proc_func_;
};

class ConstsAST : public BaseAST {
public:
    ConstsAST(VarDefList defs, unsigned int line_pos)
            : defs_(std::move(defs)) {
        set_line_pos(line_pos);
    }

    SymbolType SemaAnalyze(Analyzer &ana) override;

private:
    VarDefList defs_;
};

class VarsAST : public BaseAST {
public:
    VarsAST(VarDefList defs, unsigned int line_pos)
            : defs_(std::move(defs)) {
        set_line_pos(line_pos);
    }

    SymbolType SemaAnalyze(Analyzer &ana) override;

private:
    VarDefList defs_;
};

class ProcedureAST : public BaseAST {
public:
    ProcedureAST(const std::string &id, ASTPtr block,
            unsigned int line_pos)
            : id_(id), block_(std::move(block)) {
        set_line_pos(line_pos);
    }

    SymbolType SemaAnalyze(Analyzer &ana) override;

private:
    std::string id_;
    ASTPtr block_;
};

class FunctionAST : public BaseAST {
public:
    FunctionAST(const std::string &id, IdList args,
            ASTPtr block, unsigned int line_pos)
            : id_(id), args_(std::move(args)),
              block_(std::move(block)) {
        set_line_pos(line_pos);
    }

    SymbolType SemaAnalyze(Analyzer &ana) override;

private:
    std::string id_;
    IdList args_;
    ASTPtr block_;
};

class AssignAST : public BaseAST {
public:
    AssignAST(const std::string &id, ASTPtr expr, unsigned int line_pos)
            : id_(id), expr_(std::move(expr)) {
        set_line_pos(line_pos);
    }

    SymbolType SemaAnalyze(Analyzer &ana) override;

private:
    std::string id_;
    ASTPtr expr_;
};

class BeginEndAST: public BaseAST {
public:
    BeginEndAST(ASTPtrList stats, unsigned int line_pos)
            : stats_(std::move(stats)) {
        set_line_pos(line_pos);
    }

    SymbolType SemaAnalyze(Analyzer &ana) override;

private:
    ASTPtrList stats_;
};

class IfAST : public BaseAST {
public:
    IfAST(ASTPtr cond, ASTPtr then, ASTPtr else_then,
            unsigned int line_pos)
            : cond_(std::move(cond)), then_(std::move(then)),
              else_then_(std::move(else_then)) {
        set_line_pos(line_pos);
    }

    SymbolType SemaAnalyze(Analyzer &ana) override;

private:
    ASTPtr cond_, then_, else_then_;
};

class WhileAST : public BaseAST {
public:
    WhileAST(ASTPtr cond, ASTPtr body, unsigned int line_pos)
            : cond_(std::move(cond)), body_(std::move(body)) {
        set_line_pos(line_pos);
    }

    SymbolType SemaAnalyze(Analyzer &ana) override;

private:
    ASTPtr cond_, body_;
};

class AsmAST : public BaseAST {
public:
    AsmAST(const std::string &asm_str, unsigned int line_pos)
            : asm_str_(asm_str) {
        set_line_pos(line_pos);
    }

    SymbolType SemaAnalyze(Analyzer &ana) override;

private:
    std::string asm_str_;
};

class ControlAST : public BaseAST {
public:
    ControlAST(Lexer::Keyword type, unsigned int line_pos) : type_(type) {
        set_line_pos(line_pos);
    }

    SymbolType SemaAnalyze(Analyzer &ana) override;

private:
    Lexer::Keyword type_;
};

class UnaryAST : public BaseAST {
public:
    UnaryAST(Lexer::Keyword op, ASTPtr operand, unsigned int line_pos)
            : op_(op), operand_(std::move(operand)) {
        set_line_pos(line_pos);
    }

    SymbolType SemaAnalyze(Analyzer &ana) override;

private:
    Lexer::Keyword op_;     // 'odd' only
    ASTPtr operand_;
};

class BinaryAST : public BaseAST {
public:
    BinaryAST(Lexer::Operator op, ASTPtr lhs, ASTPtr rhs,
            unsigned int line_pos)
            : op_(op), lhs_(std::move(lhs)), rhs_(std::move(rhs)) {
        set_line_pos(line_pos);
    }

    SymbolType SemaAnalyze(Analyzer &ana) override;

private:
    Lexer::Operator op_;
    ASTPtr lhs_, rhs_;
};

class FunCallAST : public BaseAST {
public:
    FunCallAST(const std::string &id, ASTPtrList args,
            unsigned int line_pos)
            : id_(id), args_(std::move(args)) {
        set_line_pos(line_pos);
    }

    SymbolType SemaAnalyze(Analyzer &ana) override;

private:
    std::string id_;
    ASTPtrList args_;
};

class IdAST : public BaseAST {
public:
    IdAST(const std::string &id, unsigned int line_pos) : id_(id) {
        set_line_pos(line_pos);
    }

    SymbolType SemaAnalyze(Analyzer &ana) override;

private:
    std::string id_;
};

class NumberAST : public BaseAST {
public:
    NumberAST(int value, unsigned int line_pos) : value_(value) {
        set_line_pos(line_pos);
    }

    SymbolType SemaAnalyze(Analyzer &ana) override;

private:
    int value_;
};

#endif // PL01_DEFINE_AST_H_
