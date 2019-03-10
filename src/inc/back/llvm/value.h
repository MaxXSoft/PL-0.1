#ifndef PL01_BACK_LLVM_VALUE_H_
#define PL01_BACK_LLVM_VALUE_H_

#include <memory>
#include <map>
#include <string>

#include <llvm/IR/Value.h>

class ValueTable;
using VTPtr = std::shared_ptr<ValueTable>;

class ValueTable {
public:
    ValueTable() {}
    ValueTable(const VTPtr &table) : outer_(table) {}

    void AddValue(const std::string &id, llvm::Value *value) {
        values_.insert({id, value});
    }

    llvm::Value *GetValue(const std::string &id) {
        auto it = values_.find(id);
        if (it != values_.end()) {
            return it->second;
        }
        else if (outer_) {
            return outer_->GetValue(id);
        }
        else {
            return nullptr;
        }
    }

    const VTPtr &outer() const { return outer_; }

private:
    VTPtr outer_;
    std::map<std::string, llvm::Value *> values_;
};

#endif // PL01_BACK_LLVM_VALUE_H_
