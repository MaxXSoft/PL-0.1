#ifndef PL01_BACK_LLVM_BUILDER_H_
#define PL01_BACK_LLVM_BUILDER_H_

#include <memory>
#include <string>
#include <utility>
#include <stack>
#include <map>
#include <cstdlib>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Type.h>

#include <back/irbuilder.h>
#include <back/llvm/value.h>
#include <back/llvm/rawstd.h>

class LLVMIRBuilder : public IRBuilder {
public:
    LLVMIRBuilder(const std::string &name)
            : builder_(context_),
              module_(std::make_unique<llvm::Module>(name, context_)) {
        NewTable();
        InitializeFPM();
        InitializeTarget();
    }

    IRPtr GenerateBlock(LazyIRGen consts, LazyIRGen vars,
            LazyIRGen proc_func, LazyIRGen stat) override;
    IRPtr GenerateConst(const std::string &id,
            const IRPtr &expr) override;
    IRPtr GenerateVar(const std::string &id, const IRPtr &init) override;
    IRPtr GenerateProcedure(const std::string &id,
            LazyIRGen block) override;
    IRPtr GenerateFunction(const std::string &id,
            const IdList &args, LazyIRGen block) override;
    IRPtr GenerateAssign(const std::string &id,
            const IRPtr &expr, SymbolType type) override;
    IRPtr GenerateIf(const IRPtr &cond, LazyIRGen then,
            LazyIRGen else_then) override;
    IRPtr GenerateWhile(LazyIRGen cond, LazyIRGen body) override;
    IRPtr GenerateAsm(const std::string &asm_str) override;
    IRPtr GenerateControl(Lexer::Keyword type) override;
    IRPtr GenerateUnary(const IRPtr &operand) override;
    IRPtr GenerateBinary(Lexer::Operator op,
            const IRPtr &lhs, const IRPtr &rhs) override;
    IRPtr GenerateFunCall(const std::string &id,
            const IRPtrList &args) override;
    IRPtr GenerateId(const std::string &id, SymbolType type) override;
    IRPtr GenerateNumber(int value) override;

    bool CompileToObject(const char *file);
    void Dump(RawStdOStream &&os = std::cerr) {
        module_->print(os, nullptr);
    }

private:
    // pair for storing target block of break & continue
    using BreakCont = std::pair<llvm::BasicBlock *, llvm::BasicBlock *>;

    void InitializeFPM();
    void InitializeTarget();
    void OptimizeFunction(llvm::Function *func) { fpm_->run(*func); }
    llvm::AllocaInst *CreateAlloca(llvm::Function *func);
    std::string NewFunName(const std::string &id);

    template <typename... Args>
    llvm::Function *CreateFunction(const char *name, llvm::Type *ret,
            Args... args) {
        llvm::Type *args_type[] = {args...};
        auto func_type = llvm::FunctionType::get(ret, args_type, false);
        auto func = llvm::Function::Create(func_type,
                llvm::Function::ExternalLinkage, name, module_.get());
        auto body = llvm::BasicBlock::Create(context_, "", func);
        builder_.SetInsertPoint(body);
        return func;
    }

    void NewTable() { values_ = std::make_shared<ValueTable>(values_); } 
    void RestoreTable() { values_ = values_->outer(); }

    // LLVM stuffs
    llvm::LLVMContext context_;
    llvm::IRBuilder<> builder_;
    std::unique_ptr<llvm::Module> module_;
    std::unique_ptr<llvm::legacy::FunctionPassManager> fpm_;
    // stack for generating break/continue statement
    std::stack<BreakCont> break_cont_;
    // stack for current function
    std::stack<llvm::Function *> cur_func_;
    std::stack<LazyIRGen> gen_func_args_;
    // constants and variables
    VTPtr values_;
};

#endif // PL01_BACK_LLVM_BUILDER_H_
