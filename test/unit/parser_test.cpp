#include <test.h>

#include <sstream>

#include <front/parser.h>

using namespace std;

namespace {

const char *program0 = R"raw(
    const a = 1, b = 2;
    var c = 3, d = a * 2 + 1;

    procedure test;
    begin
        c := -d * 6 + 10;
        write(c)
    end;

    function test2(a);
    begin
        if a <= 10 then begin
            test2 := 1;
        end
        else if a > 100 then begin
            test2 := 2;
        end;
    end;

    begin
        test;
        test2(d + 3, e);
    end.
)raw";

const char *program1 = R"raw(
    function fib(n);
    begin
    if n < 1 then begin
        fib := -1;
    end
    else if n <= 2 then begin
        fib := 1;
    end
    else begin
        fib := fib(n - 1) + fib(n - 2);
    end;
    end;

    procedure main;
    var in;
    begin
    in := 1;
    while in >= 1 do begin
        read(in);
        write(fib(in));
    end;
    end;

    main.
)raw";

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
