#ifndef PL01_TEST_UNIT_UTIL_H_
#define PL01_TEST_UNIT_UTIL_H_

#include <type_traits>

template <typename T>
typename std::underlying_type<T>::type EnumCast(T value) {
    return static_cast<typename std::underlying_type<T>::type>(value);
}

#endif // PL01_TEST_UNIT_UTIL_H_
