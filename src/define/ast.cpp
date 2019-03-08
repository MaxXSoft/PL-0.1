#include <define/ast.h>

#include <iomanip>

namespace {

int indent_count = 0, in_expr = 0;
auto indent = [](std::ostream &os) {
    if (indent_count) os << std::setw(indent_count * 2) << ' ';
};

std::ostream &operator<<(std::ostream &os, decltype(indent) func) {
    func(os);
    return os;
}

} // namespace

void BlockAST::Dump(std::ostream &os) {
    os << indent << "BlockAST {" << std::endl;
    if (consts_) {
        ++indent_count;
        os << indent << "constants:" << std::endl;
        ++indent_count;
        consts_->Dump(os);
        indent_count -= 2;
    }
    if (vars_) {
        ++indent_count;
        os << indent << "variables:" << std::endl;
        ++indent_count;
        vars_->Dump(os);
        indent_count -= 2;
    }
    if (proc_func_.size()) {
        ++indent_count;
        os << indent << "procedures/functions:" << std::endl;
        ++indent_count;
        for (const auto &i : proc_func_) i->Dump(os);
        indent_count -= 2;
    }
    if (stat_) {
        ++indent_count;
        os << indent << "statement:" << std::endl;
        ++indent_count;
        stat_->Dump(os);
        indent_count -= 2;
    }
    os << indent << "}" << std::endl;
}

void ConstsAST::Dump(std::ostream &os) {
    for (const auto &i : defs_) {
        os << indent << i.first << " = ";
        i.second->Dump(os);
        os << std::endl;
    }
}

void VarsAST::Dump(std::ostream &os) {
    for (const auto &i : defs_) {
        os << indent << i.first;
        if (i.second) {
            os << " = ";
            i.second->Dump(os);
        }
        os << std::endl;
    }
}

void ProcedureAST::Dump(std::ostream &os) {
    os << indent << "ProcedureAST {" << std::endl;
    ++indent_count;
    os << indent << "id: " << id_ << std::endl;
    ++indent_count;
    block_->Dump(os);
    indent_count -= 2;
    os << indent << "}" << std::endl;
}

void FunctionAST::Dump(std::ostream &os) {
    os << indent << "FunctionAST {" << std::endl;
    ++indent_count;
    os << indent << "id: " << id_ << std::endl;
    if (args_.size()) {
        os << indent << "arguments: ";
        for (auto it = args_.begin(); it != args_.end(); ++it) {
            if (it != args_.begin()) os << ", ";
            os << *it;
        }
        os << std::endl;
    }
    ++indent_count;
    block_->Dump(os);
    indent_count -= 2;
    os << indent << "}" << std::endl;
}

void AssignAST::Dump(std::ostream &os) {
    os << indent << id_ << " <- ";
    ++in_expr;
    expr_->Dump(os);
    --in_expr;
    os << std::endl;
}

void BeginEndAST::Dump(std::ostream &os) {
    os << indent << "BeginEndAST {" << std::endl;
    ++indent_count;
    for (const auto &i : stats_) {
        i->Dump(os);
    };
    --indent_count;
    os << indent << "}" << std::endl;
}

void IfAST::Dump(std::ostream &os) {
    os << indent << "IfAST {" << std::endl;
    ++indent_count;
    os << indent << "cond:" << std::endl;
    ++indent_count;
    cond_->Dump(os);
    indent_count -= 2;
    if (then_) {
        ++indent_count;
        os << indent << "then-body:" << std::endl;
        ++indent_count;
        then_->Dump(os);
        indent_count -= 2;
    }
    if (else_then_) {
        ++indent_count;
        os << indent << "else-body:" << std::endl;
        ++indent_count;
        else_then_->Dump(os);
        indent_count -= 2;
    }
    os << indent << "}" << std::endl;
}

void WhileAST::Dump(std::ostream &os) {
    os << indent << "WhileAST {" << std::endl;
    ++indent_count;
    os << indent << "cond:" << std::endl;
    ++indent_count;
    cond_->Dump(os);
    indent_count -= 2;
    if (body_) {
        ++indent_count;
        os << indent << "body:" << std::endl;
        ++indent_count;
        body_->Dump(os);
        indent_count -= 2;
    }
    os << indent << "}" << std::endl;
}

void AsmAST::Dump(std::ostream &os) {
    os << indent << "AsmAST {" << std::endl;
    ++indent_count;
    os << indent;
    for (const auto &c : asm_str_) {
        os << c;
        if (c == '\n') os << indent;
    }
    --indent_count;
    os << indent << "}" << std::endl;
}

void ControlAST::Dump(std::ostream &os) {
    os << indent;
    if (type_ == Lexer::Keyword::Break) {
        os << "BreakAST {}";
    }
    else {
        os << "ContinueAST {}";
    }
    os << std::endl;
}

void UnaryAST::Dump(std::ostream &os) {
    if (!in_expr) os << indent;
    ++in_expr;
    os << "Odd(";
    operand_->Dump(os);
    os << ")";
    --in_expr;
    if (!in_expr) os << std::endl;
}

void BinaryAST::Dump(std::ostream &os) {
    using Operator = Lexer::Operator;
    if (!in_expr) os << indent;
    ++in_expr;
    switch (op_) {
        case Operator::Add: os << "Add"; break;
        case Operator::Sub: os << "Sub"; break;
        case Operator::Mul: os << "Mul"; break;
        case Operator::Div: os << "Div"; break;
        case Operator::Less: os << "Lt"; break;
        case Operator::LessEqual: os << "LE"; break;
        case Operator::Great: os << "Gt"; break;
        case Operator::GreatEqual: os << "GE"; break;
        case Operator::NotEqual: os << "NE"; break;
        case Operator::Equal: os << "Eq"; break;
        default:;
    }
    os << "(";
    lhs_->Dump(os);
    os << ", ";
    rhs_->Dump(os);
    os << ")";
    --in_expr;
    if (!in_expr) os << std::endl;
}

void FunCallAST::Dump(std::ostream &os) {
    if (!in_expr) os << indent;
    ++in_expr;
    os << id_ << "(";
    for (auto it = args_.begin(); it != args_.end(); ++it) {
        if (it != args_.begin()) os << ", ";
        (*it)->Dump(os);
    }
    os << ")";
    --in_expr;
    if (!in_expr) os << std::endl;
}

void IdAST::Dump(std::ostream &os) {
    if (!in_expr) os << indent;
    os << id_;
    if (!in_expr) os << std::endl;
}

void NumberAST::Dump(std::ostream &os) {
    os << value_;
}
