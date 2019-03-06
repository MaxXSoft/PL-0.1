#ifndef PL01_BACK_IRBUILDER_H_
#define PL01_BACK_IRBUILDER_H_

#include <back/ir.h>

class IRBuilder {
public:
    virtual ~IRBuilder() = default;

    virtual IRPtr GenerateBlock() = 0;
    virtual IRPtr GenerateNumber(int value) = 0;
};

#endif // PL01_BACK_IRBUILDER_H_
