#ifndef PL01_BACK_LLVM_IR_H_
#define PL01_BACK_LLVM_IR_H_

#include <llvm/IR/Value.h>

#include <back/ir.h>

class LLVMIR : public IRBase {
public:
    LLVMIR(llvm::Value *value) : value_(value) {}

    const std::any value() const override { return value_; }

private:
    llvm::Value *value_;
};

#endif // PL01_BACK_LLVM_IR_H_
