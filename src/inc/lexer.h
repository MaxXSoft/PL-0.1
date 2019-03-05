#ifndef PL01_LEXER_H_
#define PL01_LEXER_H_

#include <istream>
#include <string>

class Lexer {
public:
    enum class Token {
        Error, End, Char,
        Id, Num,
        Keyword, Operator
    };

    enum class Keyword {
        Const, Var, Procedure, Function, Begin, End,
        If, Then, Else, While, Do, Break, Continue,
        Odd, Asm
    };

    enum class Operator {
        Add, Sub, Mul, Div,
        Less, LessEqual, Great, GreatEqual,
        NotEqual, Equal, Assign
    };

    Lexer(std::istream &in)
            : in_(in), line_pos_(1), error_num_(0), last_char_(' ') {
        in_ >> std::noskipws;
    }

    Token NextToken();
    void Reset() {
        line_pos_ = 1;
        error_num_ = 0;
        last_char_ = ' ';
    }

    unsigned int line_pos() const { return line_pos_; }
    unsigned int error_num() const { return error_num_; }
    const std::string &id_val() const { return id_val_; }
    int num_val() const { return num_val_; }
    Keyword key_val() const { return key_val_; }
    Operator op_val() const { return op_val_; }
    char char_val() const { return char_val_; }

private:
    void NextChar() { in_ >> last_char_; }
    bool IsEOL() {
        return in_.eof() || last_char_ == '\n' || last_char_ == '\r';
    }
    Token PrintError(const char *message);
    Token HandleId();
    Token HandleNum();
    Token HandleOperator();
    Token HandleComment();
    Token HandleEOL();

    std::istream &in_;
    unsigned int line_pos_, error_num_;
    char last_char_;
    std::string id_val_;
    int num_val_;
    Keyword key_val_;
    Operator op_val_;
    char char_val_;
};

#endif // PL01_LEXER_H_
