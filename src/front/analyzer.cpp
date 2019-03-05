#include <front/analyzer.h>

#include <iostream>

namespace {

inline bool IsError(SymbolType type) {
    return type == SymbolType::Error;
}

} // namespace

SymbolType Analyzer::PrintError(const char *message,
        unsigned int line_pos) {
    std::cerr << "analyzer (line " << line_pos;
    std::cerr << "): error: " << message << std::endl;
    ++error_num_;
    return SymbolType::Error;
}

SymbolType Analyzer::PrintError(const char *message, const char *id,
        unsigned int line_pos) {
    std::cerr << "analyzer (line " << line_pos;
    std::cerr << ", id: " << id << "): error: " << message << std::endl;
    ++error_num_;
    return SymbolType::Error;
}

SymbolType Analyzer::AnalyzeConst(const std::string &id, SymbolType init,
        unsigned int line_pos) {
    if (init != SymbolType::Const) {
        return PrintError("initialize with non-constant value",
                id.c_str(), line_pos);
    }
    if (!IsError(env_->GetType(id))) {
        return PrintError("identifier has already been defined",
                id.c_str(), line_pos);
    }
    env_->AddSymbol(id, SymbolType::Const);
    return SymbolType::Void;
}

SymbolType Analyzer::AnalyzeVar(const std::string &id, SymbolType init,
        unsigned int line_pos) {
    if (init != SymbolType::Const && init != SymbolType::Var) {
        return PrintError("invalid initial value", id.c_str(), line_pos);
    }
    if (!IsError(env_->GetType(id))) {
        return PrintError("identifier has already been defined",
                id.c_str(), line_pos);
    }
    env_->AddSymbol(id, SymbolType::Var);
    return SymbolType::Void;
}

SymbolType Analyzer::AnalyzeProcedure(const std::string &id,
        unsigned int line_pos) {
    //
}

SymbolType Analyzer::AnalyzeFunction(const std::string &id,
        const IdList &args, unsigned int line_pos) {
    //
}

SymbolType Analyzer::AnalyzeAssign(const std::string &id,
        SymbolType expr_type, unsigned int line_pos) {
    //
}

SymbolType Analyzer::AnalyzeControl(unsigned int line_pos) {
    //
}

SymbolType Analyzer::AnalyzeUnary(SymbolType operand,
        unsigned int line_pos) {
    //
}

SymbolType Analyzer::AnalyzeBinary(SymbolType lhs, SymbolType rhs,
        unsigned int line_pos) {
    //
}

SymbolType Analyzer::AnalyzeFunCall(const std::string &id, int arg_count,
        unsigned int line_pos) {
    //
}

SymbolType Analyzer::AnalyzeId(const std::string &id,
        unsigned int line_pos) {
    //
}
