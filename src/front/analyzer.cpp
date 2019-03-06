#include <front/analyzer.h>

#include <iostream>

namespace {

inline bool IsError(SymbolInfo info) {
    return info.type == SymbolType::Error;
}

inline bool IsError(SymbolType type) {
    return type == SymbolType::Error;
}

inline bool IsConstOrVar(SymbolType type) {
    return type == SymbolType::Const || type == SymbolType::Var;
}

} // namespace

SymbolType Analyzer::PrintError(const char *message,
        unsigned int line_pos) {
    std::cerr << "\033[1manalyzer\033[0m (line " << line_pos;
    std::cerr << "): \033[31m\033[1merror\033[0m: " << message << std::endl;
    ++error_num_;
    return SymbolType::Error;
}

SymbolType Analyzer::PrintError(const char *message, const char *id,
        unsigned int line_pos) {
    std::cerr << "\033[1manalyzer\033[0m (line " << line_pos;
    std::cerr << ", id: " << id << "): \033[31m\033[1merror\033[0m: ";
    std::cerr << message << std::endl;
    ++error_num_;
    return SymbolType::Error;
}

SymbolType Analyzer::IsIdDefined(const std::string &id,
        unsigned int line_pos) {
    if (!IsError(env_->GetInfo(id, false))) {
        return PrintError("identifier has already been defined",
                id.c_str(), line_pos);
    }
    return SymbolType::Void;
}

SymbolType Analyzer::AnalyzeConst(const std::string &id, SymbolType init,
        unsigned int line_pos) {
    if (init != SymbolType::Const) {
        return PrintError("initialize with non-constant value",
                id.c_str(), line_pos);
    }
    if (IsError(IsIdDefined(id, line_pos))) return SymbolType::Error;
    env_->AddSymbol(id, {SymbolType::Const, 0});
    return SymbolType::Void;
}

SymbolType Analyzer::AnalyzeVar(const std::string &id,
        unsigned int line_pos) {
    if (IsError(IsIdDefined(id, line_pos))) return SymbolType::Error;
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
    if (IsError(IsIdDefined(id, line_pos))) return SymbolType::Error;
    env_->AddSymbol(id, {SymbolType::Proc, 0});
    return SymbolType::Void;
}

SymbolType Analyzer::AnalyzeFunction(const std::string &id,
        const IdList &args, unsigned int line_pos) {
    if (!IsError(env_->outer()->GetInfo(id, false))) {
        return PrintError("identifier has already been defined",
                id.c_str(), line_pos);
    }
    // add function id to current environment
    // NOTE: this id is assignable (as return value),
    //       and also callable (recursive call)
    env_->AddSymbol(id, {SymbolType::Ret, args.size()});
    // add argument id to current environment
    for (const auto &i : args) {
        if (IsError(IsIdDefined(i, line_pos))) return SymbolType::Error;
        env_->AddSymbol(i, {SymbolType::Var, 0});
    }
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
    if (info.type != SymbolType::Var && info.type != SymbolType::Ret) {
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

SymbolType Analyzer::AnalyzeFunCall(const std::string &id,
        const TypeList &args, unsigned int line_pos) {
    auto info = env_->GetInfo(id);
    if (info.type != SymbolType::Func && info.type != SymbolType::Ret) {
        return PrintError("try to call a non-function",
                id.c_str(), line_pos);
    }
    if (info.func_arg_count != args.size()) {
        return PrintError("argument count mismatch", id.c_str(), line_pos);
    }
    for (const auto &i : args) {
        if (i != SymbolType::Const && i != SymbolType::Var) {
            return PrintError("invalid argument", id.c_str(), line_pos);
        }
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
        case SymbolType::Func:
        case SymbolType::Ret: return AnalyzeFunCall(id, {}, line_pos);
        default: return info.type;
    }
}
