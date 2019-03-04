#include <lexer.h>

#include <iostream>
#include <cstring>

namespace {

const char *keywords[] = {
    "const", "var", "procedure", "function", "call", "begin", "end",
    "if", "then", "else", "while", "do", "break", "continue",
    "odd", "asm"
};

const char *operators[] = {
    "+", "-", "*", "/",
    "<", "<=", ">", ">=",
    "<>", "=", ":="
};

template <typename T>
int GetIndex(const char *str, T &str_array) {
    auto len = sizeof(str_array) / sizeof(str_array[0]);
    for (int i = 0; i < len; ++i) {
        if (!strcmp(str, str_array[i])) return i;
    }
    return -1;
}

bool IsOperatorChar(char c) {
    const char op_chars[] = "+-*/<=>:";
    for (const auto &i : op_chars) {
        if (i == c) return true;
    }
    return false;
}

} // namespace

Lexer::Token Lexer::PrintError(const char *message) {
    std::cerr << "lexer (line " << line_pos_;
    std::cerr << "): error: " << message << std::endl;
    ++error_num_;
    return Token::Error;
}

Lexer::Token Lexer::HandleId() {
    // read string
    std::string id;
    do {
        id += std::tolower(last_char_);
        NextChar();
    } while (!IsEOL() && std::isalnum(last_char_));
    // check if string is keyword
    int index = GetIndex(id.c_str(), keywords);
    if (index < 0) {
        id_val_ = id;
        return Token::Id;
    }
    else {
        key_val_ = static_cast<Keyword>(index);
        return Token::Keyword;
    }
}

Lexer::Token Lexer::HandleNum() {
    // check if is a hexadecimal literal number
    bool hex = false;
    if (last_char_ == '$') {
        hex = true;
        NextChar();
    }
    // read number string
    std::string num;
    do {
        num += last_char_;
        NextChar();
    } while (!IsEOL() && std::isxdigit(last_char_));
    // convert to integer
    char *end_pos;
    num_val_ = std::strtol(num.c_str(), &end_pos, hex ? 16 : 10);
    // check if conversion is valid
    return *end_pos ? PrintError("invalid number") : Token::Num;
}

Lexer::Token Lexer::HandleOperator() {
    // read string
    std::string op;
    do {
        op += last_char_;
        NextChar();
    } while (!IsEOL() && IsOperatorChar(last_char_));
    // check if operator is valid
    int index = GetIndex(op.c_str(), operators);
    if (index < 0) {
        return PrintError("unknown operator");
    }
    else {
        op_val_ = static_cast<Operator>(index);
        return Token::Operator;
    }
}

Lexer::Token Lexer::HandleComment() {
    NextChar();
    while (!in_.eof() && last_char_ != '}') NextChar();
    NextChar();
    return NextToken();
}

Lexer::Token Lexer::HandleEOL() {
    do {
        ++line_pos_;
        NextChar();
    } while (IsEOL() && !in_.eof());
    return NextToken();
}

Lexer::Token Lexer::NextToken() {
    // end of file
    if (in_.eof()) return Token::End;
    // skip spaces
    while (!IsEOL() && std::isspace(last_char_)) NextChar();
    // skip comment
    if (last_char_ == '{') return HandleComment();
    // id or keyword
    if (std::isalpha(last_char_)) return HandleId();
    // number
    if (std::isdigit(last_char_) || last_char_ == '$') return HandleNum();
    // operator
    if (IsOperatorChar(last_char_)) return HandleOperator();
    // end of line
    if (IsEOL()) return HandleEOL();
    // other characters
    char_val_ = last_char_;
    NextChar();
    return Token::Char;
}
