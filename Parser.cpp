#include "Parser.hpp"

inline int Parser::getLine()
{
    return currentLex->line;
}

inline int Parser::getPos()
{
    return currentLex->pos;
}

void Parser::getNextLex()
{
    int readValue;

    currentLex = lexer.getLex();

    while (currentLex == 0) {
        readValue = read(readFD, &buffer, sizeof(buffer));

        if (READ_ERROR(readValue)) {
            perror("read"); // TODO: throw
            break;
        } else if (READ_EOF(readValue)) {
            lexer.putEOF();
        } else {
            lexer.putNewData(buffer, readValue);
        }

        currentLex = lexer.getLex();
    }

    // TODO: throw if SCR_LEX_ERROR
}

bool Parser::tryLex(ScriptLexemeType type)
{
    return (currentLex->type == type);
}

bool Parser::tryLex(ScriptLexemeType type, int aIntValue)
{
    return (currentLex->type == type) &&
        (currentLex->intValue == aIntValue);
}

/* '+', '-', '!' */
bool Parser::isLexMonadicOp()
{
    return tryLex(SCR_LEX_OPERATION, OP_PLUS) ||
        tryLex(SCR_LEX_OPERATION, OP_MINUS) ||
        tryLex(SCR_LEX_OPERATION, OP_NOT);
}

/* '*', '/', '%' */
bool Parser::isLexArithmeticOp_1()
{
    return tryLex(SCR_LEX_OPERATION, OP_MULTIPLICATION) ||
        tryLex(SCR_LEX_OPERATION, OP_DIVISION) ||
        tryLex(SCR_LEX_OPERATION, OP_REMAINDER_DIVISION);
}

/* '+', '-' */
bool Parser::isLexArithmeticOp_2()
{
    return tryLex(SCR_LEX_OPERATION, OP_PLUS) ||
        tryLex(SCR_LEX_OPERATION, OP_MINUS);
}

/* '<', '>', '==' */
bool Parser::isLexCompareOp()
{
    return tryLex(SCR_LEX_OPERATION, OP_LESS) ||
        tryLex(SCR_LEX_OPERATION, OP_MORE) ||
        tryLex(SCR_LEX_OPERATION, OP_EQUALITY);
}

/* '&&' */
bool Parser::isLexLogicOp_1()
{
    return tryLex(SCR_LEX_OPERATION, OP_AND);
}

/* '||' */
bool Parser::isLexLogicOp_2()
{
    return tryLex(SCR_LEX_OPERATION, OP_OR);
}

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
                    getLine(), getPos(),
                    __FILE__, __LINE__);
            }
        } while (! tryLex(SCR_LEX_BRACKET, BRACKET_FIGURE_CLOSE));
    } else {
        SingleOperator();
        if (! tryLex(SCR_LEX_SEMICOLON)) {
                throw ParserException("Expected ';' lexeme"
                    " after SINGLE_OPERATOR.\n",
                    getLine(), getPos(),
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
            getLine(), getPos(),
            __FILE__, __LINE__);
    }
    getNextLex(); // skip '('
    if (! tryLex(SCR_LEX_BRACKET, BRACKET_PARENTHESIS_OPEN)) {
        throw ParserException("Expected ')' lexeme"
            ", this operator has no arguments.\n",
            getLine(), getPos(),
            __FILE__, __LINE__);
    }
    getNextLex(); // skip ')'
}

void Parser::ArgsList_1()
{
    if (! tryLex(SCR_LEX_BRACKET, BRACKET_PARENTHESIS_OPEN)) {
        throw ParserException("Expected '(' lexeme"
            " after operator with one argument.\n",
            getLine(), getPos(),
            __FILE__, __LINE__);
    }
    getNextLex(); // skip '('
    Expr();
    if (! tryLex(SCR_LEX_BRACKET, BRACKET_PARENTHESIS_OPEN)) {
        throw ParserException("Expected ')' lexeme"
            ", this operator has one argument.\n",
            getLine(), getPos(),
            __FILE__, __LINE__);
    }
    getNextLex(); // skip ')'
}

void Parser::ArgsList_2()
{
    if (! tryLex(SCR_LEX_BRACKET, BRACKET_PARENTHESIS_OPEN)) {
        throw ParserException("Expected '(' lexeme"
            " after operator with two arguments.\n",
            getLine(), getPos(),
            __FILE__, __LINE__);
    }
    getNextLex(); // skip '('
    Expr();
    if (! tryLex(SCR_LEX_COLON)) {
        throw ParserException("Expected ',' lexeme"
            " after first argument of operator"
            " with two arguments.\n",
            getLine(), getPos(),
            __FILE__, __LINE__);
    }
    getNextLex(); // skip ','
    Expr();
    if (! tryLex(SCR_LEX_BRACKET, BRACKET_PARENTHESIS_OPEN)) {
        throw ParserException("Expected ')' lexeme"
            ", this operator has two arguments.\n",
            getLine(), getPos(),
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

void Parser::Expr_0()
{
    if (tryLex(SCR_LEX_NUMBER)) {
        getNextLex(); // skip 'number'
    } else if (tryLex(SCR_LEX_BRACKET, BRACKET_PARENTHESIS_OPEN)) {
        getNextLex(); // skip '('
        Expr();
        if (! tryLex(SCR_LEX_BRACKET, BRACKET_PARENTHESIS_CLOSE)) {
            throw ParserException("Expected ')' lexeme"
                ", unbalanced parenthesis.\n",
                getLine(), getPos(),
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
    if (isLexMonadicOp()) {
        getNextLex();
    }

    Expr_0();
}

void Parser::Expr_2()
{
    Expr_1();

    if (isLexArithmeticOp_1()) {
        getNextLex();
        Expr_1();
    }
}

void Parser::Expr_3()
{
    Expr_2();

    if (isLexArithmeticOp_2()) {
        getNextLex();
        Expr_2();
    }
}

void Parser::Expr_4()
{
    Expr_3();

    if (isLexCompareOp()) {
        getNextLex();
        Expr_3();
    }
}

void Parser::Expr_5()
{
    Expr_4();

    if (isLexLogicOp_1()) {
        getNextLex();
        Expr_4();
    }
}

void Parser::Expr()
{
    Expr_5();

    if (isLexLogicOp_2()) {
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
            getLine(), getPos(),
            __FILE__, __LINE__);
    }
    getNextLex(); // skip 'variable'
    ArraySuffix(def);
}

/* Argument mean: "must be defined".
 * See README for more information. */
void Parser::ArraySuffix(bool def)
{
    if (! tryLex(SCR_LEX_BRACKET, BRACKET_SQUARE_OPEN))
        return; /* Do nothing */

    getNextLex(); // skip '['
    Expr();
    if (! tryLex(SCR_LEX_BRACKET, BRACKET_SQUARE_CLOSE)) {
        throw ParserException("Expected ']' lexeme"
            ", unbalanced square brackets.\n",
            getLine(), getPos(),
            __FILE__, __LINE__);
    }
    getNextLex(); // skip ']'
}

void Parser::LabelPrefix()
{
    if (! tryLex(SCR_LEX_LABEL))
        return; /* Do nothing */

    getNextLex(); // skip 'label'
    if (! tryLex(SCR_LEX_SEMICOLON)) {
        throw ParserException("Expected ':' lexeme"
            " after label (re)definition.\n",
            getLine(), getPos(),
            __FILE__, __LINE__);
    }
    getNextLex(); // skip ':'
}

Parser::Parser(int aReadFD)
    : readFD(aReadFD),
    lexer(),
    currentLex(0)
        {}

void Parser::Parse()
{
    Program();
}
