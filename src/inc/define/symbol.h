#ifndef PL01_DEFINE_SYMBOL_H_
#define PL01_DEFINE_SYMBOL_H_

#include <memory>
#include <string>
#include <map>

#include <define/type.h>
#include <define/funchand.h>

class Environment;
using EnvPtr = std::shared_ptr<Environment>;

class Environment {
public:
    Environment() : outer_(nullptr) {}
    Environment(const EnvPtr &outer) : outer_(outer) {}

    void AddSymbol(const std::string &id, SymbolType type) {
        symbols_.insert({id, {type, 0}});
    }
    void AddFunction(const std::string &id, const FunctionHandler &func) {
        symbols_.insert({id, {SymbolType::Func, func}});
    }
    SymbolType GetType(const std::string &id);

    const EnvPtr &outer() const { return outer_; }
    bool is_root() const { return outer_ == nullptr; }

private:
    struct SymbolInfo {
        SymbolType type;
        union SymbolValue {
            SymbolValue(int value) : i(value) {}
            SymbolValue(const FunctionHandler &value) : f(value) {}
            int i;
            FunctionHandler f;
        } value;
    };

    using SymbolTable = std::map<std::string, SymbolInfo>;

    EnvPtr outer_;
    SymbolTable symbols_;
};

#endif // PL01_DEFINE_SYMBOL_H_
