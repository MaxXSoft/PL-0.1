#ifndef PL01_DEFINE_TYPE_H_
#define PL01_DEFINE_TYPE_H_

#include <vector>
#include <string>

/*

type of symbol:
    Error:  sematic analysis error
    Const:  integer constant
    Var:    integer variable
    Proc:   procedure
    Func:   function that returns an integer
    Ret:    reference to a function,
            represents both the function return value and function itself
    Void:   void type (type of general statements)

*/

enum class SymbolType {
    Error, Const, Var, Proc, Func, Ret, Void
};

using IdList = std::vector<std::string>;

#endif // PL01_DEFINE_TYPE_H_
