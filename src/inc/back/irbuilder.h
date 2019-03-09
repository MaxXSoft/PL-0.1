#ifndef PL01_BACK_IRBUILDER_H_
#define PL01_BACK_IRBUILDER_H_

#include <string>

#include <front/lexer.h>
#include <back/ir.h>
#include <define/type.h>

class IRBuilder {
public:
    virtual ~IRBuilder() = default;

    virtual IRPtr GenerateBlock(LazyIRGen consts, LazyIRGen vars,
            LazyIRGen proc_func, LazyIRGen stat) = 0;
    virtual IRPtr GenerateConst(const std::string &id,
            const IRPtr &expr) = 0;
    virtual IRPtr GenerateVar(const std::string &id, const IRPtr &init) = 0;
    virtual IRPtr GenerateProcedure(const std::string &id,
            LazyIRGen block) = 0;
    virtual IRPtr GenerateFunction(const std::string &id,
            const IdList &args, LazyIRGen block) = 0;
    virtual IRPtr GenerateAssign(const std::string &id,
            const IRPtr &expr) = 0;
    virtual IRPtr GenerateIf(const IRPtr &cond, LazyIRGen then,
            LazyIRGen else_then) = 0;
    virtual IRPtr GenerateWhile(LazyIRGen cond, LazyIRGen body) = 0;
    virtual IRPtr GenerateAsm(const std::string &asm_str) = 0;
    virtual IRPtr GenerateControl(Lexer::Keyword type) = 0;
    virtual IRPtr GenerateUnary(const IRPtr &operand) = 0;  // 'odd' only
    virtual IRPtr GenerateBinary(Lexer::Operator op,
            const IRPtr &lhs, const IRPtr &rhs) = 0;
    virtual IRPtr GenerateFunCall(const std::string &id,
            const IRPtrList &args) = 0;
    virtual IRPtr GenerateId(const std::string &id) = 0;
    virtual IRPtr GenerateNumber(int value) = 0;
};

#endif // PL01_BACK_IRBUILDER_H_
