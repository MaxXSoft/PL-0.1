#ifndef PL01_TEST_TEST_H_
#define PL01_TEST_TEST_H_

// a simple unit test framework by MaxXing

#include <type_traits>
#include <iostream>
#include <cstdint>

// class definition
class Test {
public:
    template <typename T>
    static void TestExpression(bool expr, const char *expr_str,
            const T &expect, const T &actual, const char *file, int line) {
        ++test_count_;
        if (!expr) {
            std::cerr << file << ":" << line << ": in expression \"";
            std::cerr << expr_str << "\": expect: " << expect;
            std::cerr << ", actual: " << actual << std::endl;
        }
        else {
            ++pass_count_;
        }
    }

    static void ShowMessage(const char *msg);
    static void ShowResult();

    static int test_count() { return test_count_; }
    static int pass_count() { return pass_count_; }
    static bool is_passed() { return test_count_ == pass_count_; }

private:
    static int test_count_;
    static int pass_count_;
};

#define TEST_EXPECT(expect, actual) \
    do { \
        auto temp = (actual);\
        Test::TestExpression((decltype(temp)(expect)) == temp, \
                "(" #expect ") == (" #actual ")", expect, \
                temp, __FILE__, __LINE__); \
    } while (0)
#define UNIT_TEST_DECLARE(name) void name();
#define UNIT_TEST_ARRAY(name) name,
#define UNIT_TEST(tests, arr) \
    tests(UNIT_TEST_DECLARE) \
    void (*arr[])() = {tests(UNIT_TEST_ARRAY)}

#endif // PL01_TEST_TEST_H_
