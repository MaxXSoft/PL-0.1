#include <define/ast.h>

#include <cassert>

IRPtr BlockAST::GenerateIR(IRBuilder &irb) {
    return irb.GenerateBlock(
            [&] { return consts_ ? consts_->GenerateIR(irb) : nullptr; },
            [&] { return vars_ ? vars_->GenerateIR(irb) : nullptr; },
            [&] {
                for (const auto &i : proc_func_) i->GenerateIR(irb);
                return nullptr;
            },
            [&] { return stat_ ? stat_->GenerateIR(irb) : nullptr; });
}

IRPtr ConstsAST::GenerateIR(IRBuilder &irb) {
    for (const auto &i : defs_) {
        irb.GenerateConst(i.first, i.second->GenerateIR(irb));
    }
    return nullptr;
}

IRPtr VarsAST::GenerateIR(IRBuilder &irb) {
    for (const auto &i : defs_) {
        auto init = i.second ? i.second->GenerateIR(irb) : nullptr;
        irb.GenerateConst(i.first, init);
    }
    return nullptr;
}

IRPtr ProcedureAST::GenerateIR(IRBuilder &irb) {
    return irb.GenerateProcedure(id_,
            [&] { return block_->GenerateIR(irb); });
}

IRPtr FunctionAST::GenerateIR(IRBuilder &irb) {
    return irb.GenerateFunction(id_, args_,
            [&] { return block_->GenerateIR(irb); });
}

IRPtr AssignAST::GenerateIR(IRBuilder &irb) {
    return irb.GenerateAssign(id_, expr_->GenerateIR(irb));
}

IRPtr BeginEndAST::GenerateIR(IRBuilder &irb) {
    for (const auto &i : stats_) {
        i->GenerateIR(irb);
    }
    return nullptr;
}

IRPtr IfAST::GenerateIR(IRBuilder &irb) {
    return irb.GenerateIf(cond_->GenerateIR(irb),
            [&] { return then_ ? then_->GenerateIR(irb) : nullptr; },
            [&] { return else_then_ ?
                    else_then_->GenerateIR(irb) : nullptr; });
}

IRPtr WhileAST::GenerateIR(IRBuilder &irb) {
    return irb.GenerateWhile([&] { return cond_->GenerateIR(irb); },
            [&] { return body_ ? body_->GenerateIR(irb) : nullptr; });
}

IRPtr AsmAST::GenerateIR(IRBuilder &irb) {
    return irb.GenerateAsm(asm_str_);
}

IRPtr ControlAST::GenerateIR(IRBuilder &irb) {
    return irb.GenerateControl(type_);
}

IRPtr UnaryAST::GenerateIR(IRBuilder &irb) {
    static_cast<void>(op_);     // 'odd' only
    assert(op_ == Lexer::Keyword::Odd);
    return irb.GenerateUnary(operand_->GenerateIR(irb));
}

IRPtr BinaryAST::GenerateIR(IRBuilder &irb) {
    return irb.GenerateBinary(op_,
            lhs_->GenerateIR(irb), rhs_->GenerateIR((irb)));
}

IRPtr FunCallAST::GenerateIR(IRBuilder &irb) {
    IRPtrList args;
    for (const auto &i : args_) {
        args.push_back(i->GenerateIR(irb));
    }
    return irb.GenerateFunCall(id_, args);
}

IRPtr IdAST::GenerateIR(IRBuilder &irb) {
    return irb.GenerateId(id_);
}

IRPtr NumberAST::GenerateIR(IRBuilder &irb) {
    return irb.GenerateNumber(value_);
}
