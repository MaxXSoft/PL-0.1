#include <back/llvm/builder.h>

#include <vector>
#include <cassert>

#include <llvm/IR/Constants.h>
#include <llvm/ADT/APInt.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Type.h>
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
    //
    return nullptr;
}

IRPtr LLVMIRBuilder::GenerateConst(const std::string &id,
        const IRPtr &expr) {
    //
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
    //
    return nullptr;
}

IRPtr LLVMIRBuilder::GenerateWhile(const IRPtr &cond, LazyIRGen body) {
    //
    return nullptr;
}

IRPtr LLVMIRBuilder::GenerateAsm(const std::string &asm_str) {
    //
    return nullptr;
}

IRPtr LLVMIRBuilder::GenerateControl(Lexer::Keyword type) {
    //
    return nullptr;
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
    return MakeIR(nullptr);
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
    auto v = llvm::ConstantInt::get(context_, llvm::APInt(32, value, true));
    return MakeIR(v);
}
