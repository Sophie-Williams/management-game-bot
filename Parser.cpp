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

    if (currentLex->type == SCR_LEX_KEYWORD) {
        currentLex->intValue =
            tables.getKeywordType(currentLex->strValue);
        if (currentLex->intValue == SCR_KEYWORD_UNKNOWN) {
            // TODO: throw
        }
    }

#if !defined(DAEMON) && defined(PARSER_DEBUG)
    lexer.print(stderr, currentLex);
#endif

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
    } while (! tryLex(SCR_LEX_EOF));
}

void Parser::LabelOperator()
{
    LabelPrefix();
    Operator();
}

void Parser::Operator()
{
    if (tryLex(SCR_LEX_BRACKET, BRACKET_FIGURE_OPEN)) {
        getNextLex(); // skip '{'
        do {
            LabelOperator();
            if (tryLex(SCR_LEX_EOF)) {
                throw ParserException("Expected '}' lexeme"
                    " or LABEL_OPERATOR, got EOF.",
                    getLine(), getPos(),
                    __FILE__, __LINE__);
            }
        } while (! tryLex(SCR_LEX_BRACKET, BRACKET_FIGURE_CLOSE));
        getNextLex(); // skip '}'
    } else if (tryLex(SCR_LEX_KEYWORD, SCR_KEYWORD_IF)) {
        getNextLex(); // skip 'if'
        ArgsList_1();
        Operator();
        ElseSuffix();
    } else if (tryLex(SCR_LEX_KEYWORD, SCR_KEYWORD_WHILE)) {
        getNextLex(); // skip 'while'
        ArgsList_1();
        Operator();
    } else {
        SingleOperator();
        if (! tryLex(SCR_LEX_SEMICOLON)) {
                throw ParserException("Expected ';' lexeme"
                    " after SINGLE_OPERATOR.",
                    getLine(), getPos(),
                    __FILE__, __LINE__);
        }
        getNextLex(); // skip ';'
    }
}

void Parser::SingleOperator()
{
    if (! tryLex(SCR_LEX_KEYWORD) ||
        tryLex(SCR_LEX_KEYWORD, SCR_KEYWORD_ELSE))
    {
        throw ParserException("Expected operator name.",
            getLine(), getPos(),
            __FILE__, __LINE__);
    }

    ScriptLangKeywords keyword =
        static_cast<ScriptLangKeywords>(currentLex->intValue);
    getNextLex(); // skip operator name

    switch (keyword) {
    case SCR_KEYWORD_SET:
        Variable(false);
        Expr();
        break;
    case SCR_KEYWORD_GOTO:
        if (! tryLex(SCR_LEX_LABEL)) {
            throw ParserException("Expected label.",
                getLine(), getPos(),
                __FILE__, __LINE__);
        }
        getNextLex(); // skip 'label'
    case SCR_KEYWORD_IF:
    case SCR_KEYWORD_ELSE:
    case SCR_KEYWORD_WHILE:
        /* Not possible */
        break;
    case SCR_KEYWORD_PRINT:
        if (! tryLex(SCR_LEX_BRACKET, BRACKET_PARENTHESIS_OPEN)) {
            throw ParserException("Expected '(' lexeme"
                " after print operator name.",
                getLine(), getPos(),
                __FILE__, __LINE__);
        }
        getNextLex(); // skip '('
        PrintArgsList();
        if (! tryLex(SCR_LEX_BRACKET, BRACKET_PARENTHESIS_CLOSE)) {
            throw ParserException("Expected ')' lexeme"
                " after arguments of print operator.",
                getLine(), getPos(),
                __FILE__, __LINE__);
        }
        getNextLex(); // skip ')'
        break;
    case SCR_KEYWORD_BUY:
    case SCR_KEYWORD_SELL:
        ArgsList_2();
        break;
    case SCR_KEYWORD_MAKE:
    case SCR_KEYWORD_BUILD:
        ArgsList_1();
        break;
    case SCR_KEYWORD_TURN:
        ArgsList_0();
        break;
    case SCR_KEYWORD_UNKNOWN:
        /* Not possible */
        break;
    }
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
            " after operator or function with no arguments.",
            getLine(), getPos(),
            __FILE__, __LINE__);
    }
    getNextLex(); // skip '('
    if (! tryLex(SCR_LEX_BRACKET, BRACKET_PARENTHESIS_CLOSE)) {
        throw ParserException("Expected ')' lexeme"
            ", this operator or function has no arguments.",
            getLine(), getPos(),
            __FILE__, __LINE__);
    }
    getNextLex(); // skip ')'
}

void Parser::ArgsList_1()
{
    if (! tryLex(SCR_LEX_BRACKET, BRACKET_PARENTHESIS_OPEN)) {
        throw ParserException("Expected '(' lexeme"
            " after operator or function with one argument.",
            getLine(), getPos(),
            __FILE__, __LINE__);
    }
    getNextLex(); // skip '('
    Expr();
    if (! tryLex(SCR_LEX_BRACKET, BRACKET_PARENTHESIS_CLOSE)) {
        throw ParserException("Expected ')' lexeme"
            ", this operator or function has one argument.",
            getLine(), getPos(),
            __FILE__, __LINE__);
    }
    getNextLex(); // skip ')'
}

void Parser::ArgsList_2()
{
    if (! tryLex(SCR_LEX_BRACKET, BRACKET_PARENTHESIS_OPEN)) {
        throw ParserException("Expected '(' lexeme"
            " after operator with two arguments.",
            getLine(), getPos(),
            __FILE__, __LINE__);
    }
    getNextLex(); // skip '('
    Expr();
    if (! tryLex(SCR_LEX_COMMA)) {
        throw ParserException("Expected ',' lexeme"
            " after first argument of operator"
            " with two arguments.",
            getLine(), getPos(),
            __FILE__, __LINE__);
    }
    getNextLex(); // skip ','
    Expr();
    if (! tryLex(SCR_LEX_BRACKET, BRACKET_PARENTHESIS_CLOSE)) {
        throw ParserException("Expected ')' lexeme"
            ", this operator has two arguments.",
            getLine(), getPos(),
            __FILE__, __LINE__);
    }
    getNextLex(); // skip ')'
}

void Parser::PrintArgsList()
{
    PrintArg();

    while (tryLex(SCR_LEX_COMMA)) {
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
                ", unbalanced parenthesis.",
                getLine(), getPos(),
                __FILE__, __LINE__);
        }
        getNextLex(); // skip ')'
    } else if (tryLex(SCR_LEX_FUNCTION)) {
        // TODO: ArgsList_0 or ArgsList_1 via tables
        if (STR_EQUAL("production", currentLex->strValue) ||
            STR_EQUAL("raw", currentLex->strValue))
        {
            getNextLex(); // skip function name
            ArgsList_1();
        } else {
            getNextLex(); // skip function name
            ArgsList_0();
        }
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
        throw ParserException("Expected variable.",
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
            ", unbalanced square brackets.",
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
    if (! tryLex(SCR_LEX_COLON)) {
        throw ParserException("Expected ':' lexeme"
            " after label (re)definition.",
            getLine(), getPos(),
            __FILE__, __LINE__);
    }
    getNextLex(); // skip ':'
}

Parser::Parser(int aReadFD)
    : readFD(aReadFD),
    lexer(),
    currentLex(0),
    tables()
        {}

void Parser::parse()
{
    getNextLex(); // get first lexeme
    Program();
}
