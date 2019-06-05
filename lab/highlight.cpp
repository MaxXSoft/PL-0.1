#include <fstream>
#include <iostream>
#include <iomanip>
using namespace std;

#include <front/lexer.h>

namespace {

using Token = Lexer::Token;

enum FontColor {
    kColorRed = 31,
    kColorGreen, kColorYellow, kColorBlue,
    kColorPurple, kColorCyan, kColorLightGray,
};

void PrintText(const string &str, int color = 0, bool bold = false) {
    string temp;
    if (bold || color) {
        if (bold) cout << "\033[1m";
        if (color) cout << "\033[" << color << "m";
        cout << str << "\033[0m";
    }
    else {
        cout << str;
    }
}

const char *keywords[] = {
    "const", "var", "procedure", "function", "begin", "end",
    "if", "then", "else", "while", "do", "break", "continue",
    "odd", "asm"
};

const char *operators[] = {
    "+", "-", "*", "/",
    "<", "<=", ">", ">=",
    "<>", "=", ":="
};

} // namespace

int main(int argc, const char *argv[]) {
    if (argc < 2) return 1;
    ifstream ifs(argv[1]);
    Lexer lexer(ifs);
    unsigned int last_line = 0;
    for (;;) {
        auto tok = lexer.NextToken();
        if (lexer.line_pos() != last_line) {
            if (last_line) cout << endl;
            last_line = lexer.line_pos();
            cout << setw(5) << left << last_line;
        }
        switch (tok) {
            case Token::Error: return lexer.error_num();
            case Token::End: {
                PrintText("EOF\n", 0, true);
                return 0;
            }
            case Token::Id: {
                PrintText(lexer.id_val(), kColorCyan);
                break;
            }
            case Token::Num: {
                cout << lexer.num_val();
                break;
            }
            case Token::String: {
                PrintText(lexer.str_val(), kColorYellow);
                break;
            }
            case Token::Keyword: {
                auto str = keywords[static_cast<int>(lexer.key_val())];
                PrintText(str, kColorGreen);
                break;
            }
            case Token::Operator: {
                auto str = operators[static_cast<int>(lexer.op_val())];
                PrintText(str, kColorPurple);
                break;
            }
            case Token::Char: {
                cout << lexer.char_val();
                break;
            }
        }
        cout << ' ';
    }
    return 0;
}
