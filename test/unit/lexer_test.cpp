#include <test.h>

#include <sstream>

#include <lexer.h>
#include <unit/util.h>

using namespace std;

namespace {

using Token = Lexer::Token;
using Keyword = Lexer::Keyword;
using Operator = Lexer::Operator;

} // namespace

void LexerTest() {
    // initialize lexer
    istringstream iss;
    Lexer lexer(iss);
    // test lexer
    iss.str("");
    iss.clear();
    TEST_EXPECT(EnumCast(Token::End), EnumCast(lexer.NextToken()));
    iss.str("const a01 = 100, bAAb = $66;\n\n { comment }");
    iss.clear();
    lexer.Reset();
    TEST_EXPECT(EnumCast(Token::Keyword), EnumCast(lexer.NextToken()));
    TEST_EXPECT(EnumCast(Keyword::Const), EnumCast(lexer.key_val()));
    TEST_EXPECT(EnumCast(Token::Id), EnumCast(lexer.NextToken()));
    TEST_EXPECT("a01"s, lexer.id_val());
    TEST_EXPECT(EnumCast(Token::Operator), EnumCast(lexer.NextToken()));
    TEST_EXPECT(EnumCast(Operator::Equal), EnumCast(lexer.op_val()));
    TEST_EXPECT(EnumCast(Token::Num), EnumCast(lexer.NextToken()));
    TEST_EXPECT(100, lexer.num_val());
    TEST_EXPECT(EnumCast(Token::Char), EnumCast(lexer.NextToken()));
    TEST_EXPECT(',', lexer.char_val());
    TEST_EXPECT(EnumCast(Token::Id), EnumCast(lexer.NextToken()));
    TEST_EXPECT("baab"s, lexer.id_val());
    TEST_EXPECT(EnumCast(Token::Operator), EnumCast(lexer.NextToken()));
    TEST_EXPECT(EnumCast(Operator::Equal), EnumCast(lexer.op_val()));
    TEST_EXPECT(EnumCast(Token::Num), EnumCast(lexer.NextToken()));
    TEST_EXPECT(0x66, lexer.num_val());
    TEST_EXPECT(EnumCast(Token::Char), EnumCast(lexer.NextToken()));
    TEST_EXPECT(';', lexer.char_val());
    TEST_EXPECT(EnumCast(Token::End), EnumCast(lexer.NextToken()));
    TEST_EXPECT(0U, lexer.error_num());
    TEST_EXPECT(3U, lexer.line_pos());
    iss.str("0ea");
    iss.clear();
    lexer.Reset();
    TEST_EXPECT(EnumCast(Token::Error), EnumCast(lexer.NextToken()));
    iss.str("0123");
    iss.clear();
    lexer.Reset();
    TEST_EXPECT(EnumCast(Token::Error), EnumCast(lexer.NextToken()));
    iss.str("::=");
    iss.clear();
    lexer.Reset();
    TEST_EXPECT(EnumCast(Token::Error), EnumCast(lexer.NextToken()));
}
