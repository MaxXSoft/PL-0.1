#include <fstream>
using namespace std;

#include <front/lexer.h>
#include <front/parser.h>

int main(int argc, const char *argv[]) {
    if (argc < 2) return 1;
    ifstream ifs(argv[1]);
    Lexer lexer(ifs);
    Parser parser(lexer);
    auto ast = parser.ParseProgram();
    ast->Dump();
    return 0;
}
