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
    auto &entry = func->getEntryBlock();
    llvm::IRBuilder<> builder(&entry, entry.begin());
    return builder.CreateAlloca(builder_.getInt32Ty());
}

std::string LLVMIRBuilder::NewFunName(const std::string &id) {
    // avoid naming conflict when creating a function called main
    return id != "main" ? id : "_main";
}

IRPtr LLVMIRBuilder::GenerateBlock(LazyIRGen consts, LazyIRGen vars,
        LazyIRGen proc_func, LazyIRGen stat) {
    bool is_func_declare = !consts && !vars && !stat;
    // generate procdures and functions first
    proc_func();
    // check if is body of function or procedure
    if (!cur_func_.empty() && !is_func_declare) {
        auto body = llvm::BasicBlock::Create(context_, "", cur_func_.top());
        builder_.SetInsertPoint(body);
    }
    // generate constants and variables
    if (consts) consts();
    if (vars) vars();
    // generate arguments and return value of function if necessary
    if (!gen_func_args_.empty() && !is_func_declare) gen_func_args_.top()();
    // generate statement
    if (stat) {
        // check if it's need to generate main function
        if (cur_func_.empty()) {
            auto func = CreateFunction("main",
                    builder_.getInt32Ty(), builder_.getInt32Ty(),
                    builder_.getInt8PtrTy()->getPointerTo());
            cur_func_.push(func);
            stat();
            builder_.CreateRet(builder_.getInt32(0));
            cur_func_.pop();
            OptimizeFunction(func);
        }
        else {
            stat();
        }
    }
    return nullptr;
}

IRPtr LLVMIRBuilder::GenerateConst(const std::string &id,
        const IRPtr &expr) {
    values_->AddValue(id, GetValue(expr));
    return nullptr;
}

IRPtr LLVMIRBuilder::GenerateVar(const std::string &id, const IRPtr &init) {
    llvm::Value *var;
    auto init_value = init ? GetValue(init) : builder_.getInt32(0);
    if (cur_func_.empty()) {
        // create global variable
        var = module_->getOrInsertGlobal(id, builder_.getInt32Ty());
    }
    else {
        // create alloca
        var = CreateAlloca(cur_func_.top());
    }
    // generate store
    builder_.CreateStore(init_value, var);
    values_->AddValue(id, var);
    return nullptr;
}

IRPtr LLVMIRBuilder::GenerateProcedure(const std::string &id,
        LazyIRGen block) {
    // TODO: nested function!
    // create function declaraction
    auto func_type = llvm::FunctionType::get(builder_.getVoidTy(), false);
    auto func = llvm::Function::Create(func_type,
            llvm::Function::ExternalLinkage, NewFunName(id), module_.get());
    // store information of current function
    cur_func_.push(func);
    values_->AddValue(id, func);
    NewTable();
    // generate block
    bool is_declare = true;
    gen_func_args_.push([&is_declare] {
        is_declare = false;
        return nullptr;
    });
    block();
    // generate return statement
    if (!is_declare) builder_.CreateRetVoid();
    // remove current function info
    cur_func_.pop();
    RestoreTable();
    // do optimize
    OptimizeFunction(func);
    return nullptr;
}

IRPtr LLVMIRBuilder::GenerateFunction(const std::string &id,
        const IdList &args, LazyIRGen block) {
    // TODO: nested function!
    // create function declaraction
    std::vector<llvm::Type *> args_type(args.size(), builder_.getInt32Ty());
    auto func_type = llvm::FunctionType::get(builder_.getInt32Ty(),
            args_type, false);
    auto func = llvm::Function::Create(func_type,
            llvm::Function::ExternalLinkage, NewFunName(id), module_.get());
    // store information of current function
    cur_func_.push(func);
    values_->AddValue(id, func);
    NewTable();
    // generate arguments and return value
    llvm::Value *ret = nullptr;
    gen_func_args_.push([this, func, &args, &id, &ret] {
        auto it = args.begin();
        for (auto &&arg : func->args()) {
            auto alloca = CreateAlloca(func);
            builder_.CreateStore(&arg, alloca);
            values_->AddValue(*it, alloca);
            ++it;
        }
        ret = CreateAlloca(func);
        builder_.CreateStore(builder_.getInt32(0), ret);
        values_->AddValue(id + "_ret", ret);
        return nullptr;
    });
    // generate block
    block();
    // generate return statement if not a function declare
    if (ret) builder_.CreateRet(builder_.CreateLoad(ret));
    // remove current function info
    cur_func_.pop();
    RestoreTable();
    gen_func_args_.pop();
    // do optimize
    OptimizeFunction(func);
    return nullptr;
}

IRPtr LLVMIRBuilder::GenerateAssign(const std::string &id,
        const IRPtr &expr, SymbolType type) {
    llvm::Value *ptr = nullptr;
    if (type == SymbolType::Ret) {
        ptr = values_->GetValue(id + "_ret");
    }
    else {
        ptr = values_->GetValue(id);
    }
    assert(ptr);
    builder_.CreateStore(GetValue(expr), ptr);
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
    if (then) then();
    builder_.CreateBr(merge_block);
    // emit 'else' block
    cur_func->getBasicBlockList().push_back(else_block);
    builder_.SetInsertPoint(else_block);
    if (else_then) else_then();
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
    // create direct branch
    builder_.CreateBr(cond_block);
    // emit 'cond' block
    builder_.SetInsertPoint(cond_block);
    auto cond_expr = GetValue(cond());
    builder_.CreateCondBr(cond_expr, body_block, end_block);
    // emit 'body' block
    cur_func->getBasicBlockList().push_back(body_block);
    builder_.SetInsertPoint(body_block);
    if (body) body();
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
    auto callee = values_->GetValue(id);
    assert(callee);
    // get value list
    std::vector<llvm::Value *> values;
    for (const auto &i : args) {
        values.push_back(GetValue(i));
    }
    // generate ir
    return MakeIR(builder_.CreateCall(callee, values));
}

IRPtr LLVMIRBuilder::GenerateId(const std::string &id, SymbolType type) {
    if (type == SymbolType::Proc || type == SymbolType::Func
            || type == SymbolType::Ret) {
        return GenerateFunCall(id, {});
    }
    else {
        auto value = values_->GetValue(id);
        assert(value);
        return MakeIR(builder_.CreateLoad(value));
    }
}

IRPtr LLVMIRBuilder::GenerateNumber(int value) {
    return MakeIR(builder_.getInt32(value));
}
