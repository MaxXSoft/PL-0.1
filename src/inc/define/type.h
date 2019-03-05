#ifndef PL01_DEFINE_TYPE_H_
#define PL01_DEFINE_TYPE_H_

#include <vector>
#include <string>

enum class SymbolType {
    Error, Const, Var, Func, Void
};

using IdList = std::vector<std::string>;

#endif // PL01_DEFINE_TYPE_H_
