#ifndef PL01_DEFINE_IR_H_
#define PL01_DEFINE_IR_H_

#include <memory>

class IRBase {
public:
    virtual ~IRBase() = default;
};

using IRPtr = std::shared_ptr<IRBase>;

#endif // PL01_DEFINE_IR_H_
