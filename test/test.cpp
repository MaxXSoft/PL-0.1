#include <test.h>

#include <cstdio>

using namespace std;

int Test::test_count_ = 0;
int Test::pass_count_ = 0;

void Test::ShowMessage(const char *msg) {
    printf("%s\n", msg);
}

void Test::ShowResult() {
    printf("%d/%d (%3.2f%%) passed\n",
            pass_count_, test_count_, pass_count_ * 100.0 / test_count_);
}
