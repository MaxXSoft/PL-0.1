#include <back/llvm/builder.h>

#include <vector>
#include <cassert>

#include <llvm/IR/Constants.h>
#include <llvm/ADT/APInt.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/InlineAsm.h>
#include <llvm/Transforms/Utils.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>

#include <back/llvm/ir.h>

namespace {

inline llvm::Value *GetValue(const IRPtr &ir) {
    assert(ir);
    auto v = IRCast<llvm::Value *>(ir);
    assert(v);
    return v;
}

inline IRPtr MakeIR(llvm::Value *value) {
    return std::make_shared<LLVMIR>(value);
}

} // namespace

void LLVMIRBuilder::InitializeFPM() {
    using namespace llvm;
    fpm_ = std::make_unique<legacy::FunctionPassManager>(module_.get());
    // allocas to registers
    fpm_->add(createPromoteMemoryToRegisterPass());
    // peephole optimizations
    fpm_->add(createInstructionCombiningPass());
    // reassociate expressions
    fpm_->add(createReassociatePass());
    // eliminate common sub-expressions
    fpm_->add(createGVNPass());
    // simplify the control flow graph
    fpm_->add(createCFGSimplificationPass());
    fpm_->doInitialization();
}

llvm::AllocaInst *LLVMIRBuilder::CreateAlloca(llvm::Function *func) {
    llvm::IRBuilder<> builder(&func->getEntryBlock(),
            func->getEntryBlock().begin());
    return builder.CreateAlloca(llvm::Type::getInt32Ty(context_));
}

IRPtr LLVMIRBuilder::GenerateBlock(LazyIRGen consts, LazyIRGen vars,
        LazyIRGen proc_func, LazyIRGen stat) {
    // check if is body of function or procedure
    if (!cur_func_.empty()) {
        auto body = llvm::BasicBlock::Create(context_, "", cur_func_.top());
        builder_.SetInsertPoint(body);
    }
    // generate constants, variables, procedures and functions
    if (consts) consts();
    if (vars) vars();
    proc_func();
    // generate statement
    if (stat) {
        // check if it's need to generate main function
        if (cur_func_.empty()) {
            auto func = CreateFunction("main",
                    builder_.getInt32Ty(), builder_.getInt32Ty(),
                    builder_.getInt8PtrTy()->getPointerTo());
            cur_func_.push(func);
            stat();
            cur_func_.pop();
            fpm_->run(*func);
        }
        else {
            stat();
        }
    }
    return nullptr;
}

IRPtr LLVMIRBuilder::GenerateConst(const std::string &id,
        const IRPtr &expr) {
    //
    // const_map_[]
    return nullptr;
}

IRPtr LLVMIRBuilder::GenerateVar(const std::string &id, const IRPtr &init) {
    //
    return nullptr;
}

IRPtr LLVMIRBuilder::GenerateProcedure(const std::string &id,
        LazyIRGen block) {
    //
    return nullptr;
}

IRPtr LLVMIRBuilder::GenerateFunction(const std::string &id,
        const IdList &args, LazyIRGen block) {
    //
    return nullptr;
}

IRPtr LLVMIRBuilder::GenerateAssign(const std::string &id,
        const IRPtr &expr) {
    //
    return nullptr;
}

IRPtr LLVMIRBuilder::GenerateIf(const IRPtr &cond, LazyIRGen then,
        LazyIRGen else_then) {
    auto cur_func = builder_.GetInsertBlock()->getParent();
    // create basic blocks
    auto then_block = llvm::BasicBlock::Create(context_, "", cur_func);
    auto else_block = llvm::BasicBlock::Create(context_);
    auto merge_block = llvm::BasicBlock::Create(context_);
    // create conditional branch
    builder_.CreateCondBr(GetValue(cond), then_block, else_block);
    // emit 'then' block
    builder_.SetInsertPoint(then_block);
    then();
    builder_.CreateBr(merge_block);
    // emit 'else' block
    cur_func->getBasicBlockList().push_back(else_block);
    builder_.SetInsertPoint(else_block);
    else_then();
    builder_.CreateBr(merge_block);
    // emit merge block
    cur_func->getBasicBlockList().push_back(merge_block);
    builder_.SetInsertPoint(merge_block);
    return nullptr;
}

IRPtr LLVMIRBuilder::GenerateWhile(LazyIRGen cond, LazyIRGen body) {
    auto cur_func = builder_.GetInsertBlock()->getParent();
    // create basic blocks
    auto cond_block = llvm::BasicBlock::Create(context_, "", cur_func);
    auto body_block = llvm::BasicBlock::Create(context_);
    auto end_block = llvm::BasicBlock::Create(context_);
    // add to break/continue stack
    break_cont_.push({end_block, cond_block});
    // emit 'cond' block
    builder_.SetInsertPoint(cond_block);
    auto cond_expr = GetValue(cond());
    builder_.CreateCondBr(cond_expr, body_block, end_block);
    // emit 'body' block
    cur_func->getBasicBlockList().push_back(body_block);
    builder_.SetInsertPoint(body_block);
    body();
    builder_.CreateBr(cond_block);
    // emit 'end' block
    cur_func->getBasicBlockList().push_back(end_block);
    builder_.SetInsertPoint(end_block);
    // pop the top element of break/continue stack
    break_cont_.pop();
    return nullptr;
}

IRPtr LLVMIRBuilder::GenerateAsm(const std::string &asm_str) {
    auto asm_ty = llvm::FunctionType::get(builder_.getVoidTy(), false);
    auto asm_func = llvm::InlineAsm::get(asm_ty, asm_str, "", true);
    return MakeIR(builder_.CreateCall(asm_func));
}

IRPtr LLVMIRBuilder::GenerateControl(Lexer::Keyword type) {
    assert(!break_cont_.empty());
    auto target = type == Lexer::Keyword::Break ?
            break_cont_.top().first : break_cont_.top().second;
    return MakeIR(builder_.CreateBr(target));
}

IRPtr LLVMIRBuilder::GenerateUnary(const IRPtr &operand) {
    // 'odd' only
    return MakeIR(builder_.CreateAnd(GetValue(operand), 1));
}

IRPtr LLVMIRBuilder::GenerateBinary(Lexer::Operator op,
        const IRPtr &lhs, const IRPtr &rhs) {
    using Operator = Lexer::Operator;
    // get lhs & rhs
    auto l = GetValue(lhs), r = GetValue(rhs);
    // generate ir
    switch (op) {
        case Operator::Add: return MakeIR(builder_.CreateAdd(l, r));
        case Operator::Sub: return MakeIR(builder_.CreateSub(l, r));
        case Operator::Mul: return MakeIR(builder_.CreateMul(l, r));
        case Operator::Div: return MakeIR(builder_.CreateSDiv(l, r));
        case Operator::Less: return MakeIR(builder_.CreateICmpSLT(l, r));
        case Operator::LessEqual:
                return MakeIR(builder_.CreateICmpSLE(l, r));
        case Operator::Great: return MakeIR(builder_.CreateICmpSGT(l, r));
        case Operator::GreatEqual:
                return MakeIR(builder_.CreateICmpSGE(l, r));
        case Operator::NotEqual: return MakeIR(builder_.CreateICmpNE(l, r));
        case Operator::Equal: return MakeIR(builder_.CreateICmpEQ(l, r));
        default:;
    }
    // error or impossible situation
    assert(false);
    return nullptr;
}

IRPtr LLVMIRBuilder::GenerateFunCall(const std::string &id,
        const IRPtrList &args) {
    // get function from module
    auto callee = module_->getFunction(id);
    assert(callee);
    // get value list
    std::vector<llvm::Value *> values;
    for (const auto &i : args) {
        values.push_back(GetValue(i));
    }
    // generate ir
    return MakeIR(builder_.CreateCall(callee, values));
}

IRPtr LLVMIRBuilder::GenerateId(const std::string &id) {
    //
    return nullptr;
}

IRPtr LLVMIRBuilder::GenerateNumber(int value) {
    return MakeIR(builder_.getInt32(value));
}
