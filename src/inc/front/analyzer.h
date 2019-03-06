#ifndef PL01_FRONT_ANALYZER_H_
#define PL01_FRONT_ANALYZER_H_

#include <string>

#include <define/type.h>
#include <define/symbol.h>

class Analyzer {
public:
    Analyzer() : env_(std::make_shared<Environment>()),
            error_num_(0), while_count_(0) {}
    Analyzer(const EnvPtr &env)
            : env_(env), error_num_(0), while_count_(0) {}

    SymbolType AnalyzeConst(const std::string &id, SymbolType init,
            unsigned int line_pos);
    SymbolType AnalyzeVar(const std::string &id, unsigned int line_pos);
    SymbolType AnalyzeVar(const std::string &id, SymbolType init,
            unsigned int line_pos);
    SymbolType AnalyzeProcedure(const std::string &id,
            unsigned int line_pos);
    SymbolType AnalyzeFunction(const std::string &id, const IdList &args,
            unsigned int line_pos);
    SymbolType AnalyzeAssign(const std::string &id, SymbolType expr_type,
            unsigned int line_pos);
    SymbolType AnalyzeControl(unsigned int line_pos);
    SymbolType AnalyzeUnary(SymbolType operand, unsigned int line_pos);
    SymbolType AnalyzeBinary(SymbolType lhs, SymbolType rhs,
            unsigned int line_pos);
    SymbolType AnalyzeFunCall(const std::string &id, int arg_count,
            unsigned int line_pos);
    SymbolType AnalyzeId(const std::string &id, unsigned int line_pos);

    void NewEnvironment() { env_ = std::make_shared<Environment>(env_); }
    void RestoreEnvironment() { env_ = env_->outer(); }
    void EnterWhile() { ++while_count_; }
    void ExitWhile() { --while_count_; }

    const EnvPtr &env() const { return env_; }
    unsigned int error_num() const { return error_num_; }

private:
    SymbolType PrintError(const char *message, unsigned int line_pos);
    SymbolType PrintError(const char *message, const char *id,
            unsigned int line_pos);
    SymbolType IsIdDefined(const std::string &id, unsigned int line_pos);

    EnvPtr env_;
    unsigned int error_num_;
    int while_count_;
};

#endif // PL01_FRONT_ANALYZER_H_
