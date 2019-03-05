#include <test.h>

#include <sstream>

#include <front/parser.h>

using namespace std;

namespace {

const char *program0 =
        "const a = 1, b = 2;\n"
        "var c = 3, d = a * 2 + 1;\n"
        "\n"
        "procedure test;\n"
        "begin\n"
        "   c := -d * 6 + 10;\n"
        "   write(c)\n"
        "end;\n"
        "\n"
        "function test2(a);\n"
        "begin\n"
        "   if a <= 10 then begin\n"
        "       test2 := 1;\n"
        "   end\n"
        "   else if a > 100 then begin\n"
        "       test2 := 2;\n"
        "   end;\n"
        "end;\n"
        "\n"
        "begin\n"
        "   test;\n"
        "   test2(d + 3, e);\n"
        "end.";

const char *program1 =
        "function fib(n);\n"
        "begin\n"
        "   if n < 1 then begin\n"
        "       fib := -1;\n"
        "   end\n"
        "   else if n <= 2 then begin\n"
        "       fib := 1;\n"
        "   end\n"
        "   else begin\n"
        "       fib := fib(n - 1) + fib(n - 2);\n"
        "   end;\n"
        "end;\n"
        "\n"
        "procedure main;\n"
        "var in;\n"
        "begin\n"
        "   in := 1;\n"
        "   while in >= 1 do begin\n"
        "       read(in);\n"
        "       write(fib(in));\n"
        "   end;\n"
        "end;\n"
        "\n"
        "main.";

} // namespace

void ParserTest() {
    // initialize parser
    istringstream iss;
    Lexer lexer(iss);
    Parser parser(lexer);
    // test parser
    iss.str(program0);
    iss.clear();
    parser.Reset();
    auto ast = parser.ParseProgram();
    TEST_EXPECT(false, ast == nullptr);
    TEST_EXPECT(0U, lexer.error_num() + parser.error_num());
    iss.str(program1);
    iss.clear();
    parser.Reset();
    ast = parser.ParseProgram();
    TEST_EXPECT(false, ast == nullptr);
    TEST_EXPECT(0U, lexer.error_num() + parser.error_num());
}
