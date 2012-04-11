#include "Parser.hpp"

// TODO: errors in ScriptLexer as exceptions.
// Or check after new lexeme get.
// TODO: maybe :%s/getNextLex/skipLex/gc
// TODO: maybe :%s/tryLex/lexIs/gc or lexCompare

void Parser::Program()
{
    do {
        LabelOperator();
    } while (tryLex(SCR_LEX_EOF));
}

void Parser::LabelOperator()
{
    LabelPrefix();
    Operator();
}

void Parser::Operator()
{
    if (tryLex(SCR_LEX_BRACKET, BRACKET_FIGURE_OPEN)) {
        do {
            LabelOperator();
            if (! tryLex(SCR_LEX_EOF)) {
                throw ParserException("Expected '}' lexeme"
                    " or LABEL_OPERATOR, got EOF.\n",
                    __FILE__, __LINE__); /* TODO: line and pos */
            }
        } while (! tryLex(SCR_LEX_BRACKET, BRACKET_FIGURE_CLOSE));
    } else {
        SingleOperator();
        if (! tryLex(SCR_LEX_SEMICOLON)) {
                throw ParserException("Expected ';' lexeme"
                    " after SINGLE_OPERATOR.\n",
                    __FILE__, __LINE__);
        }
    }
    getNextLex(); // skip '}' or ';'
}

void Parser::SingleOperator()
{
    /* TODO */
}

void Parser::ElseSuffix()
{
    if (! tryLex(SCR_LEX_KEYWORD, SCR_KEYWORD_ELSE))
        return; /* Do nothing */

    getNextLex(); // skip 'else'
    Operator();
}

void Parser::ArgsList_0()
{
    if (! tryLex(SCR_LEX_BRACKET, BRACKET_PARENTHESIS_OPEN)) {
        throw ParserException("Expected '(' lexeme"
            " after operator with no arguments.\n",
            __FILE__, __LINE__);
    }
    getNextLex(); // skip '('
    if (! tryLex(SCR_LEX_BRACKET, BRACKET_PARENTHESIS_OPEN)) {
        throw ParserException("Expected ')' lexeme"
            ", this operator has no arguments.\n",
            __FILE__, __LINE__);
    }
    getNextLex(); // skip ')'
}

void Parser::ArgsList_1()
{
    if (! tryLex(SCR_LEX_BRACKET, BRACKET_PARENTHESIS_OPEN)) {
        throw ParserException("Expected '(' lexeme"
            " after operator with one argument.\n",
            __FILE__, __LINE__);
    }
    getNextLex(); // skip '('
    Expr();
    if (! tryLex(SCR_LEX_BRACKET, BRACKET_PARENTHESIS_OPEN)) {
        throw ParserException("Expected ')' lexeme"
            ", this operator has one argument.\n",
            __FILE__, __LINE__);
    }
    getNextLex(); // skip ')'
}

void Parser::ArgsList_2()
{
    if (! tryLex(SCR_LEX_BRACKET, BRACKET_PARENTHESIS_OPEN)) {
        throw ParserException("Expected '(' lexeme"
            " after operator with two arguments.\n",
            __FILE__, __LINE__);
    }
    getNextLex(); // skip '('
    Expr();
    if (! tryLex(SCR_LEX_COLON)) {
        throw ParserException("Expected ',' lexeme"
            " after first argument of operator"
            " with two arguments.\n",
            __FILE__, __LINE__);
    }
    getNextLex(); // skip ','
    Expr();
    if (! tryLex(SCR_LEX_BRACKET, BRACKET_PARENTHESIS_OPEN)) {
        throw ParserException("Expected ')' lexeme"
            ", this operator has two arguments.\n",
            __FILE__, __LINE__);
    }
    getNextLex(); // skip ')'
}

void Parser::PrintArgsList()
{
    PrintArg();

    while (tryLex(SCR_LEX_COLON)) {
        getNextLex(); // skip ','
        PrintArg();
    }
}

void Parser::PrintArg()
{
    if (tryLex(SCR_LEX_STRING)) {
        getNextLex(); // skip 'string'
    } else {
        Expr();
    }
}

void Expr_0()
{
    if (tryLex(SCR_LEX_NUMBER)) {
        getNextLex(); // skip 'number'
    } else if (tryLex(SCR_LEX_BRACKET, BRACKET_PARENTHESIS_OPEN)) {
        getNextLex(); // skip '('
        Expr();
        if (! tryLex(SCR_LEX_BRACKET, BRACKET_PARENTHESIS_CLOSE)) {
            throw ParserException("Expected ')' lexeme"
                ", unbalanced parenthesis.\n"
                __FILE__, __LINE__);
        }
        getNextLex(); // skip ')'
    } else {
        // true argument mean that variable must be defined
        Variable(true);
    }
}

void Parser::Expr_1()
{
    if (onMonadicOp()) {
        getNextLex();
    }

    Expr_0();
}

void Parser::Expr_2()
{
    Expr_1();

    if (isArithmeticOp_1()) {
        getNextLex();
        Expr_1();
    }
}

void Parser::Expr_3()
{
    Expr_2();

    if (isArithmeticOp_2()) {
        getNextLex();
        Expr_2();
    }
}

void Parser::Expr_4()
{
    Expr_3();

    if (isCompareOp()) {
        getNextLex();
        Expr_3();
    }
}

void Parser::Expr_5()
{
    Expr_4();

    if (isLogicOp_1()) {
        getNextLex();
        Expr_4();
    }
}

void Parser::Expr()
{
    Expr_5();

    if (isLogicOp_2()) {
        getNextLex();
        Expr_5();
    }
}

/* Argument mean: "must be defined".
 * See README for more information. */
void Parser::Variable(bool def)
{
    if (! tryLex(SCR_LEX_VARIABLE)) {
        throw ParserException("Expected variable.\n",
            __FILE__, __LINE__);
    }
    getNextLex(); // skip 'variable'
    ArraySuffix(def);
}

/* Argument mean: "must be defined".
 * See README for more information. */
void Parser::ArraySuffix(bool def)
{
    if (! tryLex(SCR_LEX_BRACKET, BRACKET_SQUARE_OPEN)) {
        return; /* Do nothing */

    getNextLex(); // skip '['
    Expr();
    if (! tryLex(SCR_LEX_BRACKET, BRACKET_SQUARE_CLOSE)) {
        throw ParserException("Expected ']' lexeme"
            ", unbalanced square brackets.\n",
            __FILE__, __LINE__);
    }
    getNextLex(); // skip ']'
}

void Parser::LabelPrefix()
{
    if (! tryLex(SCR_LEX_LABEL)) {
        return; /* Do nothing */

    getNextLex(); // skip 'label'
    if (! tryLex(SCR_LEX_SEMICOLON)) {
        throw ParserException("Expected ':' lexeme"
            " after label (re)definition.\n");
    }
    getNextLex(); // skip ':'
}
