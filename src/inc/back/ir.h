#ifndef PL01_DEFINE_IR_H_
#define PL01_DEFINE_IR_H_

#include <memory>
#include <any>
#include <cassert>

class IRBase {
public:
    virtual ~IRBase() = default;

    virtual const std::any value() const = 0;
};

using IRPtr = std::shared_ptr<IRBase>;

template <typename T>
const T IRCast(const IRPtr &ir) {
    auto ret = ir->value();
    auto value = std::any_cast<T>(&ret);
    assert(value != nullptr);
    return *value;
}

#endif // PL01_DEFINE_IR_H_
