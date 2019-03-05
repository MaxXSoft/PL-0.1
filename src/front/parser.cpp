#include <front/parser.h>

#include <iostream>
#include <sstream>

namespace {

const char *keywords[] = {
    "const", "var", "procedure", "function", "begin", "end",
    "if", "then", "else", "while", "do", "break", "continue",
    "odd", "asm"
};

} // namespace

ASTPtr Parser::PrintError(const char *message) {
    std::cerr << "parser (line " << lexer_.line_pos();
    std::cerr << "): error: " << message << std::endl;
    ++error_num_;
    return nullptr;
}

ASTPtr Parser::ParseBlock() {
    ASTPtr consts, vars, stat;
    ASTPtrList proc_func;
    // get constant definition
    if (IsTokenKeyword(Keyword::Const)) {
        consts = ParseConstants();
        if (error_num_) return nullptr;
    }
    // get variable definition
    if (IsTokenKeyword(Keyword::Var)) {
        vars = ParseVariables();
        if (error_num_) return nullptr;
    }
    // get procedure/function declaration
    if (IsTokenKeyword(Keyword::Procedure)
            || IsTokenKeyword(Keyword::Function)) {
        ASTPtr pf;
        do {
            if (lexer_.key_val() == Keyword::Procedure) {
                pf = ParseProcedure();
            }
            else if (lexer_.key_val() == Keyword::Function) {
                pf = ParseFunction();
            }
            else {
                break;
            }
            if (error_num_) return nullptr;
            proc_func.push_back(std::move(pf));
        } while (cur_token_ == Token::Keyword);
    }
    // parse statements
    stat = ParseStatement();
    if (error_num_) return nullptr;
    return std::make_unique<BlockAST>(std::move(consts), std::move(vars),
            std::move(proc_func), std::move(stat), lexer_.line_pos());
}

ASTPtr Parser::ParseConstants() {
    VarDefList defs;
    do {
        // get identifier
        if (NextToken() != Token::Id) {
            return PrintError("identifier required");
        }
        auto id = lexer_.id_val();
        NextToken();
        // eat '='
        if (!IsTokenOperator(Operator::Equal)) {
            return PrintError("'=' required");
        }
        NextToken();
        // get initialize expression
        auto expr = ParseExpression();
        if (error_num_) return nullptr;
        // add to definitions
        defs.push_back({id, std::move(expr)});
    } while (IsTokenChar(','));
    // check ';'
    if (!IsTokenChar(';')) return PrintError("';' required");
    NextToken();
    return std::make_unique<ConstsAST>(std::move(defs), lexer_.line_pos());
}

ASTPtr Parser::ParseVariables() {
    VarDefList defs;
    do {
        // get identifier
        if (NextToken() != Token::Id) {
            return PrintError("identifier required");
        }
        auto id = lexer_.id_val();
        NextToken();
        // check if has initializer
        ASTPtr init;
        if (IsTokenOperator(Operator::Equal)) {
            NextToken();    // eat '='
            init = ParseExpression();
            if (error_num_) return nullptr;
        }
        // add to definitions
        defs.push_back({id, std::move(init)});
    } while (IsTokenChar(','));
    // check ';'
    if (!IsTokenChar(';')) return PrintError("';' required");
    NextToken();
    return std::make_unique<VarsAST>(std::move(defs), lexer_.line_pos());
}

ASTPtr Parser::ParseProcedure() {
    // get identifier
    if (NextToken() != Token::Id) return PrintError("identifier required");
    auto id = lexer_.id_val();
    NextToken();
    // check ';'
    if (!IsTokenChar(';')) return PrintError("';' required");
    NextToken();
    // get block
    auto block = ParseBlock();
    if (error_num_) return nullptr;
    // check ';'
    if (!IsTokenChar(';')) return PrintError("';' required");
    NextToken();
    return std::make_unique<ProcedureAST>(id,
            std::move(block), lexer_.line_pos());
}

ASTPtr Parser::ParseFunction() {
    // get identifier
    if (NextToken() != Token::Id) return PrintError("identifier required");
    auto id = lexer_.id_val();
    NextToken();
    // check if has argument list
    IdList args;
    if (IsTokenChar('(')) {
        do {
            if (NextToken() != Token::Id) {
                return PrintError("identifier required in argument list");
            }
            args.push_back(lexer_.id_val());
            NextToken();
        } while (IsTokenChar(','));
        if (!IsTokenChar(')')) return PrintError("')' required");
        NextToken();
    }
    // check ';'
    if (!IsTokenChar(';')) return PrintError("';' required");
    NextToken();
    // get block
    auto block = ParseBlock();
    if (error_num_) return nullptr;
    // check ';'
    if (!IsTokenChar(';')) return PrintError("';' required");
    NextToken();
    return std::make_unique<FunctionAST>(id, std::move(args),
            std::move(block), lexer_.line_pos());
}

// NOTE: return value is NULLABLE
ASTPtr Parser::ParseStatement() {
    switch (cur_token_) {
        case Token::Id: return ParseIdStat();
        case Token::Keyword: {
            switch (lexer_.key_val()) {
                case Keyword::Begin: return ParseBeginEnd();
                case Keyword::If: return ParseIf();
                case Keyword::While: return ParseWhile();
                case Keyword::Asm: return ParseAsm();
                case Keyword::Break:
                case Keyword::Continue: return ParseControl();
                default: return nullptr;
            }
        }
        default: return nullptr;
    }
}

ASTPtr Parser::ParseIdStat() {
    // get identifier
    auto id = lexer_.id_val();
    NextToken();
    // check next token
    if (IsTokenOperator(Operator::Assign)) {
        // assign statement
        NextToken();
        // get expression
        auto expr = ParseExpression();
        if (error_num_) return nullptr;
        return std::make_unique<AssignAST>(id,
                std::move(expr), lexer_.line_pos());
    }
    else if (IsTokenChar('(')) {
        // function call
        auto funcall = ParseFunCall(id);
        if (error_num_) return nullptr;
        return funcall;
    }
    else {
        // just identifier
        return std::make_unique<IdAST>(id, lexer_.line_pos());
    }
}

ASTPtr Parser::ParseFunCall(const std::string &id) {
    ASTPtrList args;
    // get argument list
    do {
        NextToken();
        auto expr = ParseExpression();
        if (error_num_) return nullptr;
        args.push_back(std::move(expr));
    } while (IsTokenChar(','));
    // check ')'
    if (!IsTokenChar(')')) {
        return PrintError("')' required in function call");
    }
    NextToken();
    return std::make_unique<FunCallAST>(id,
            std::move(args), lexer_.line_pos());
}

ASTPtr Parser::ParseBeginEnd() {
    ASTPtrList stats;
    // get statement list
    do {
        NextToken();
        auto stat = ParseStatement();
        if (error_num_) return nullptr;
        if (stat) stats.push_back(std::move(stat));
    } while (IsTokenChar(';'));
    // check 'end'
    if (!IsTokenKeyword(Keyword::End)) return PrintError("'end' required");
    NextToken();
    return std::make_unique<BeginEndAST>(std::move(stats),
            lexer_.line_pos());
}

ASTPtr Parser::ParseIf() {
    // eat 'if'
    NextToken();
    // get condition
    auto cond = ParseCondition();
    if (error_num_) return nullptr;
    // check 'then'
    if (!IsTokenKeyword(Keyword::Then)) {
        return PrintError("'then' required");
    }
    NextToken();
    // get then-body
    auto then = ParseStatement();
    if (error_num_) return nullptr;
    // check if has 'else'
    ASTPtr else_then;
    if (IsTokenKeyword(Keyword::Else)) {
        NextToken();
        else_then = ParseStatement();
        if (error_num_) return nullptr;
    }
    return std::make_unique<IfAST>(std::move(cond), std::move(then),
            std::move(else_then), lexer_.line_pos());
}

ASTPtr Parser::ParseWhile() {
    // eat 'while'
    NextToken();
    // get condition
    auto cond = ParseCondition();
    if (error_num_) return nullptr;
    // check 'do'
    if (!IsTokenKeyword(Keyword::Do)) return PrintError("'do' required");
    NextToken();
    // get body
    auto body = ParseStatement();
    if (error_num_) return nullptr;
    return std::make_unique<WhileAST>(std::move(cond),
            std::move(body), lexer_.line_pos());
}

ASTPtr Parser::ParseAsm() {
    // eat 'asm'
    NextToken();
    // check 'begin'
    if (!IsTokenKeyword(Keyword::Begin)) {
        return PrintError("'begin' required");
    }
    NextToken();
    // get assembly
    std::ostringstream oss;
    while (!IsTokenKeyword(Keyword::End)) {
        switch (cur_token_) {
            case Token::Id: {
                oss << lexer_.id_val() << ' ';
                break;
            }
            case Token::Keyword: {
                auto index = static_cast<int>(lexer_.key_val());
                oss << keywords[index] << ' ';
                break;
            }
            case Token::Num: {
                oss << lexer_.num_val();
                break;
            }
            case Token::Char: {
                if (lexer_.char_val() == ';') {
                    oss << std::endl;
                }
                else {
                    oss << lexer_.char_val() << ' ';
                }
                break;
            }
            default: {
                return PrintError("invalid assembly");
            }
        }
        NextToken();
    }
    // eat 'end'
    oss << std::endl;
    NextToken();
    return std::make_unique<AsmAST>(oss.str(), lexer_.line_pos());
}

ASTPtr Parser::ParseControl() {
    auto type = lexer_.key_val();
    NextToken();
    return std::make_unique<ControlAST>(type, lexer_.line_pos());
}

ASTPtr Parser::ParseCondition() {
    if (IsTokenKeyword(Keyword::Odd)) {
        // get 'odd' expression
        NextToken();
        auto expr = ParseExpression();
        if (error_num_) return nullptr;
        return std::make_unique<UnaryAST>(Keyword::Odd,
                std::move(expr), lexer_.line_pos());
    }
    else {
        // get relational expression
        auto lhs = ParseExpression();
        if (error_num_) return nullptr;
        if (!IsRelationalOp()) {
            return PrintError("relational operator required");
        }
        auto op = lexer_.op_val();
        NextToken();
        auto rhs = ParseExpression();
        if (error_num_) return nullptr;
        return std::make_unique<BinaryAST>(op,
                std::move(lhs), std::move(rhs), lexer_.line_pos());
    }
}

ASTPtr Parser::ParseExpression() {
    // check if has '+' or '-'
    Operator op;
    bool has_head_op;
    if ((has_head_op = IsAddSub())) {
        op = lexer_.op_val();
        NextToken();
    }
    // get term
    auto term = ParseTerm();
    if (error_num_) return nullptr;
    if (has_head_op) {
        auto zero = std::make_unique<NumberAST>(0, lexer_.line_pos());
        term = std::make_unique<BinaryAST>(op,
                std::move(zero), std::move(term), lexer_.line_pos());
    }
    // get rest terms
    while (IsAddSub()) {
        op = lexer_.op_val();
        NextToken();
        auto rhs = ParseTerm();
        if (error_num_) return nullptr;
        term = std::make_unique<BinaryAST>(op,
                std::move(term), std::move(rhs), lexer_.line_pos());
    }
    return term;
}

ASTPtr Parser::ParseTerm() {
    // get factor
    auto factor = ParseFactor();
    if (error_num_) return nullptr;
    // get rest factors
    while (IsMulDiv()) {
        auto op = lexer_.op_val();
        NextToken();
        auto rhs = ParseFactor();
        if (error_num_) return nullptr;
        factor = std::make_unique<BinaryAST>(op,
                std::move(factor), std::move(rhs), lexer_.line_pos());
    }
    return factor;
}

ASTPtr Parser::ParseFactor() {
    switch (cur_token_) {
        case Token::Id: {
            auto id = lexer_.id_val();
            NextToken();
            if (IsTokenChar('(')) {
                // function call
                auto funcall = ParseFunCall(id);
                if (error_num_) return nullptr;
                return funcall;
            }
            else {
                // just identifier
                return std::make_unique<IdAST>(id, lexer_.line_pos());
            }
        }
        case Token::Num: {
            auto value = lexer_.num_val();
            NextToken();
            return std::make_unique<NumberAST>(value, lexer_.line_pos());
        }
        default: {
            if (IsTokenChar('(')) {
                NextToken();
                auto expr = ParseExpression();
                if (error_num_) return nullptr;
                if (!IsTokenChar(')')) return PrintError("')' required");
                NextToken();
                return expr;
            }
            else {
                return PrintError("invalid factor");
            }
        }
    }
}

ASTPtr Parser::ParseProgram() {
    auto block = ParseBlock();
    if (error_num_) return nullptr;
    if (!IsTokenChar('.') || NextToken() != Token::End) {
        return PrintError("source program must end with '.'");
    }
    else {
        return block;
    }
}
