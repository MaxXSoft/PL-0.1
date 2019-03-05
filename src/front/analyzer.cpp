#include <front/analyzer.h>

#include <iostream>

namespace {

inline bool IsError(SymbolInfo info) {
    return info.type == SymbolType::Error;
}

inline bool IsConstOrVar(SymbolType type) {
    return type == SymbolType::Const || type == SymbolType::Var;
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
    if (!IsError(env_->GetInfo(id))) {
        return PrintError("identifier has already been defined",
                id.c_str(), line_pos);
    }
    env_->AddSymbol(id, {SymbolType::Const, 0});
    return SymbolType::Void;
}

SymbolType Analyzer::AnalyzeVar(const std::string &id,
        unsigned int line_pos) {
    if (!IsError(env_->GetInfo(id))) {
        return PrintError("identifier has already been defined",
                id.c_str(), line_pos);
    }
    env_->AddSymbol(id, {SymbolType::Var, 0});
    return SymbolType::Void;
}

SymbolType Analyzer::AnalyzeVar(const std::string &id, SymbolType init,
        unsigned int line_pos) {
    if (!IsConstOrVar(init)) {
        return PrintError("invalid initial value", id.c_str(), line_pos);
    }
    return AnalyzeVar(id, line_pos);
}

SymbolType Analyzer::AnalyzeProcedure(const std::string &id,
        unsigned int line_pos) {
    if (!IsError(env_->GetInfo(id))) {
        return PrintError("identifier has already been defined",
                id.c_str(), line_pos);
    }
    env_->AddSymbol(id, {SymbolType::Proc, 0});
    return SymbolType::Void;
}

SymbolType Analyzer::AnalyzeFunction(const std::string &id,
        const IdList &args, unsigned int line_pos) {
    if (!IsError(env_->GetInfo(id))) {
        return PrintError("identifier has already been defined",
                id.c_str(), line_pos);
    }
    // add argument id to current environment
    for (const auto &i : args) env_->AddSymbol(i, {SymbolType::Var, 0});
    // add function id to outer environment
    env_->outer()->AddSymbol(id, {SymbolType::Func, args.size()});
    return SymbolType::Void;
}

SymbolType Analyzer::AnalyzeAssign(const std::string &id,
        SymbolType expr_type, unsigned int line_pos) {
    auto info = env_->GetInfo(id);
    if (IsError(info)) {
        return PrintError("identifier has not been defined",
                id.c_str(), line_pos);
    }
    if (info.type != SymbolType::Var) {
        return PrintError("try to assign a value to a non-variable",
                id.c_str(), line_pos);
    }
    if (!IsConstOrVar(expr_type)) {
        return PrintError("invalid assignment", id.c_str(), line_pos);
    }
    return SymbolType::Void;
}

SymbolType Analyzer::AnalyzeControl(unsigned int line_pos) {
    if (!while_count_) {
        return PrintError("try to use break/continue outside 'while' loop",
                line_pos);
    }
    return SymbolType::Void;
}

SymbolType Analyzer::AnalyzeUnary(SymbolType operand,
        unsigned int line_pos) {
    if (!IsConstOrVar(operand)) {
        return PrintError("invalid operand", line_pos);
    }
    return operand;
}

SymbolType Analyzer::AnalyzeBinary(SymbolType lhs, SymbolType rhs,
        unsigned int line_pos) {
    if (!IsConstOrVar(lhs)) return PrintError("invalid lhs", line_pos);
    if (!IsConstOrVar(rhs)) return PrintError("invalid rhs", line_pos);
    if (lhs == SymbolType::Const && rhs == SymbolType::Const) {
        return SymbolType::Const;
    }
    return SymbolType::Var;
}

SymbolType Analyzer::AnalyzeFunCall(const std::string &id, int arg_count,
        unsigned int line_pos) {
    auto info = env_->GetInfo(id);
    if (info.type != SymbolType::Func) {
        return PrintError("try to call a non-function",
                id.c_str(), line_pos);
    }
    if (info.func_arg_count != arg_count) {
        return PrintError("argument count mismatch", id.c_str(), line_pos);
    }
    return SymbolType::Var;
}

SymbolType Analyzer::AnalyzeId(const std::string &id,
        unsigned int line_pos) {
    auto info = env_->GetInfo(id);
    if (IsError(info)) {
        return PrintError("identifier has not been defined",
                id.c_str(), line_pos);
    }
    switch (info.type) {
        case SymbolType::Proc: return SymbolType::Void;
        case SymbolType::Func: return AnalyzeFunCall(id, 0, line_pos);
        default: return info.type;
    }
}
