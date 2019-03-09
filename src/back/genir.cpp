#include <define/ast.h>

#include <cassert>

IRPtr BlockAST::GenerateIR(IRBuilder &irb) {
    LazyIRGen consts, vars, stat;
    if (consts_) consts = [&] { return consts_->GenerateIR(irb); };
    if (vars_) vars = [&] { return vars_->GenerateIR(irb); };
    if (stat_) stat = [&] { return stat_->GenerateIR(irb); };
    return irb.GenerateBlock(consts, vars, [&] {
                for (const auto &i : proc_func_) i->GenerateIR(irb);
                return nullptr;
            }, stat);
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
    LazyIRGen then, else_then;
    if (then_) then = [&] { return then_->GenerateIR(irb); };
    if (else_then_) else_then = [&] { return else_then_->GenerateIR(irb); };
    return irb.GenerateIf(cond_->GenerateIR(irb), then, else_then);
}

IRPtr WhileAST::GenerateIR(IRBuilder &irb) {
    LazyIRGen body;
    if (body_) body = [&] { return body_->GenerateIR(irb); };
    return irb.GenerateWhile([&] { return cond_->GenerateIR(irb); }, body);
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
