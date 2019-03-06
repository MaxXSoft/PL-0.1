#include <define/ast.h>

namespace {

inline bool IsError(SymbolType type) {
    return type == SymbolType::Error;
}

} // namespace

SymbolType BlockAST::SemaAnalyze(Analyzer &ana) {
    ana.NewEnvironment();
    if (consts_ && IsError(consts_->SemaAnalyze(ana))) {
        return SymbolType::Error;
    }
    if (vars_ && IsError(vars_->SemaAnalyze(ana))) {
        return SymbolType::Error;
    }
    for (const auto &i : proc_func_) {
        if (IsError(i->SemaAnalyze(ana))) return SymbolType::Error;
    }
    if (stat_ && IsError(stat_->SemaAnalyze(ana))) {
        return SymbolType::Error;
    }
    ana.RestoreEnvironment();
    set_env(ana.env());
    return SymbolType::Void;
}

SymbolType ConstsAST::SemaAnalyze(Analyzer &ana) {
    for (const auto &i : defs_) {
        auto ret = ana.AnalyzeConst(i.first,
                i.second->SemaAnalyze(ana), line_pos());
        if (IsError(ret)) return SymbolType::Error;
    }
    set_env(ana.env());
    return SymbolType::Void;
}

SymbolType VarsAST::SemaAnalyze(Analyzer &ana) {
    for (const auto &i : defs_) {
        SymbolType ret;
        if (i.second) {
            auto init = i.second->SemaAnalyze(ana);
            ret = ana.AnalyzeVar(i.first, init, line_pos());
        }
        else {
            ret = ana.AnalyzeVar(i.first, line_pos());
        }
        if (IsError(ret)) return SymbolType::Error;
    }
    set_env(ana.env());
    return SymbolType::Void;
}

SymbolType ProcedureAST::SemaAnalyze(Analyzer &ana) {
    if (IsError(ana.AnalyzeProcedure(id_, line_pos()))
            || IsError(block_->SemaAnalyze(ana))) {
        return SymbolType::Error;
    }
    set_env(ana.env());
    return SymbolType::Void;
}

SymbolType FunctionAST::SemaAnalyze(Analyzer &ana) {
    ana.NewEnvironment();
    if (IsError(ana.AnalyzeFunction(id_, args_, line_pos()))
            || IsError(block_->SemaAnalyze(ana))) {
        return SymbolType::Error;
    }
    ana.RestoreEnvironment();
    set_env(ana.env());
    return SymbolType::Void;
}

SymbolType AssignAST::SemaAnalyze(Analyzer &ana) {
    auto ret = ana.AnalyzeAssign(id_, expr_->SemaAnalyze(ana), line_pos());
    set_env(ana.env());
    return ret;
}

SymbolType BeginEndAST::SemaAnalyze(Analyzer &ana) {
    for (const auto &i : stats_) {
        if (IsError(i->SemaAnalyze(ana))) return SymbolType::Error;
    }
    set_env(ana.env());
    return SymbolType::Void;
}

SymbolType IfAST::SemaAnalyze(Analyzer &ana) {
    if (IsError(cond_->SemaAnalyze(ana))) return SymbolType::Error;
    if (IsError(then_->SemaAnalyze(ana))) return SymbolType::Error;
    if (else_then_ && IsError(else_then_->SemaAnalyze(ana))) {
        return SymbolType::Error;
    }
    set_env(ana.env());
    return SymbolType::Void;
}

SymbolType WhileAST::SemaAnalyze(Analyzer &ana) {
    ana.EnterWhile();
    if (IsError(cond_->SemaAnalyze(ana))) return SymbolType::Error;
    if (IsError(body_->SemaAnalyze(ana))) return SymbolType::Error;
    ana.ExitWhile();
    set_env(ana.env());
    return SymbolType::Void;
}

SymbolType AsmAST::SemaAnalyze(Analyzer &ana) {
    set_env(ana.env());
    return SymbolType::Void;
}

SymbolType ControlAST::SemaAnalyze(Analyzer &ana) {
    auto ret = ana.AnalyzeControl(line_pos());
    set_env(ana.env());
    return ret;
}

SymbolType UnaryAST::SemaAnalyze(Analyzer &ana) {
    auto ret = ana.AnalyzeUnary(operand_->SemaAnalyze(ana), line_pos());
    set_env(ana.env());
    return ret;
}

SymbolType BinaryAST::SemaAnalyze(Analyzer &ana) {
    auto ret = ana.AnalyzeBinary(lhs_->SemaAnalyze(ana),
            rhs_->SemaAnalyze(ana), line_pos());
    set_env(ana.env());
    return ret;
}

SymbolType FunCallAST::SemaAnalyze(Analyzer &ana) {
    for (const auto &i : args_) {
        if (IsError(i->SemaAnalyze(ana))) return SymbolType::Error;
    }
    auto ret = ana.AnalyzeFunCall(id_, args_.size(), line_pos());
    set_env(ana.env());
    return ret;
}

SymbolType IdAST::SemaAnalyze(Analyzer &ana) {
    auto ret = ana.AnalyzeId(id_, line_pos());
    set_env(ana.env());
    return ret;
}

SymbolType NumberAST::SemaAnalyze(Analyzer &ana) {
    set_env(ana.env());
    return SymbolType::Const;
}
