#ifndef PL01_DEFINE_SYMBOL_H_
#define PL01_DEFINE_SYMBOL_H_

#include <memory>
#include <string>
#include <map>
#include <cstdint>

#include <define/type.h>

struct SymbolInfo {
    SymbolType type;
    size_t func_arg_count;
};

class Environment;
using EnvPtr = std::shared_ptr<Environment>;

class Environment {
public:
    Environment() : outer_(nullptr) {}
    Environment(const EnvPtr &outer) : outer_(outer) {}

    void AddSymbol(const std::string &id, SymbolInfo info) {
        symbols_.insert({id, info});
    }
    void AddClosureSymbol(const std::string &id) {
        closure_symbols_.push_back(id);
    }
    SymbolInfo GetInfo(const std::string &id, bool recursive = true);

    const EnvPtr &outer() const { return outer_; }
    const IdList &closure_symbols() const { return closure_symbols_; }
    bool is_root() const { return outer_ == nullptr; }
    bool is_main() const { return outer_ && outer_->is_root(); }
    bool in_global_body() const {
        /*
            var a, b;       <- main env
            function func   <- main env
                (x);        <- global function arg env
                    var v;  <- global function body env
        */
        return outer_ && outer_->outer_ && outer_->outer_->is_main();
    }
    bool in_nested_body() const { return !is_main() && !in_global_body(); }

private:
    EnvPtr outer_;
    std::map<std::string, SymbolInfo> symbols_;
    IdList closure_symbols_;
};

#endif // PL01_DEFINE_SYMBOL_H_
