#ifndef PL01_DEFINE_SYMBOL_H_
#define PL01_DEFINE_SYMBOL_H_

#include <memory>
#include <string>
#include <map>

#include <define/funchand.h>

enum class SymbolType {
    Error, Const, Var, Func, Void
};

class Environment;
using EnvPtr = std::shared_ptr<Environment>;

class Environment {
public:
    Environment() : outer_(nullptr) {}
    Environment(const EnvPtr &outer) : outer_(outer) {}

    void InsertSymbol(const std::string &id,
            SymbolType type, int value = 0);
    void AddFunction(const std::string &id, const FunctionHandler &func);

    const EnvPtr &outer() const { return outer_; }
    bool is_root() const { return outer_ == nullptr; }

private:
    struct SymbolInfo {
        SymbolType type;
        union {
            int i;
            FunctionHandler f;
        } value;
    };

    using SymbolTable = std::map<std::string, SymbolInfo>;

    EnvPtr outer_;
    SymbolTable symbols_;
};

#endif // PL01_DEFINE_SYMBOL_H_
