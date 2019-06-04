#include <iostream>
#include <fstream>
#include <iomanip>
using namespace std;

#include <front/lexer.h>

namespace {

using Token = Lexer::Token;

template <typename T, typename U>
void PrintTableItem(const T &type, const U &value) {
    cout << setw(15) << type << setw(15) << value << endl;
}

} // namespace

int main(int argc, const char *argv[]) {
    if (argc < 2) return 1;
    ifstream ifs(argv[1]);
    Lexer lexer(ifs);
    // print title
    PrintTableItem("TOKEN TYPE", "VALUE");
    // print tokens
    for (;;) {
        auto tok = lexer.NextToken();
        switch (tok) {
            case Token::Error: return lexer.error_num();
            case Token::End: {
                PrintTableItem("End", "N/A");
                return 0;
            }
            case Token::Char: {
                PrintTableItem("Char", lexer.char_val());
                break;
            }
            case Token::Id: {
                PrintTableItem("Id", lexer.id_val());
                break;
            }
            case Token::Num: {
                PrintTableItem("Num", lexer.num_val());
                break;
            }
            case Token::String: {
                PrintTableItem("String", lexer.str_val());
                break;
            }
            case Token::Keyword: {
                PrintTableItem("Keyword", static_cast<int>(lexer.key_val()));
                break;
            }
            case Token::Operator: {
                PrintTableItem("Operator", static_cast<int>(lexer.op_val()));
                break;
            }
            default:;
        }
    }
    return 0;
}
