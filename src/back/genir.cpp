#include <define/ast.h>

IRPtr BlockAST::GenerateIR(IRBuilder &irb) {
    return irb.GenerateBlock([&] { return consts_->GenerateIR(irb); },
            [&] { return vars_->GenerateIR(irb); },
            [&] {
                for (const auto &i : proc_func_) i->GenerateIR(irb);
                return nullptr;
            },
            [&] { return stat_->GenerateIR(irb); });
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
            [&] { return then_->GenerateIR(irb); },
            [&] { return else_then_->GenerateIR(irb); });
}

IRPtr WhileAST::GenerateIR(IRBuilder &irb) {
    return irb.GenerateWhile(cond_->GenerateIR(irb),
            [&] { return body_->GenerateIR(irb); });
}

IRPtr AsmAST::GenerateIR(IRBuilder &irb) {
    return irb.GenerateAsm(asm_str_);
}

IRPtr ControlAST::GenerateIR(IRBuilder &irb) {
    return irb.GenerateControl(type_);
}

IRPtr UnaryAST::GenerateIR(IRBuilder &irb) {
    static_cast<void>(op_);     // 'odd' only
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
