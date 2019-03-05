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
    SymbolInfo GetInfo(const std::string &id);

    const EnvPtr &outer() const { return outer_; }
    bool is_root() const { return outer_ == nullptr; }

private:
    EnvPtr outer_;
    std::map<std::string, SymbolInfo> symbols_;
};

#endif // PL01_DEFINE_SYMBOL_H_
