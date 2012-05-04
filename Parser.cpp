#include "Parser.hpp"

inline int Parser::getLine() const
{
    return currentLex->line;
}

inline int Parser::getPos() const
{
    return currentLex->pos;
}

void Parser::getNextLex()
{
    int readValue;

    currentLex = lexer.getLex();

    while (currentLex == 0) {
        readValue = read(readFD, &buffer, sizeof(buffer));

        try {
            if (READ_ERROR(readValue)) {
                // TODO: IOException?
                throw ParserException("Read error.",
                    getLine(), getPos(),
                    __FILE__, __LINE__);
            } else if (READ_EOF(readValue)) {
                lexer.putEOF();
            } else {
                lexer.putNewData(buffer, readValue);
            }
        } catch(CharQueueException& ex) {
            throw ParserException(ex, 0, 0, __FILE__, __LINE__);
        }

        currentLex = lexer.getLex();
    }

    if (currentLex->type == SCR_LEX_KEYWORD) {
        try {
            currentLex->intValue =
                tables.getKeywordType(currentLex->strValue);
        } catch(TableAccessException& ex) {
            throw ParserException(ex,
                getLine(), getPos(),
                __FILE__, __LINE__);
        }
    }

#if !defined(DAEMON) && defined(PARSER_DEBUG)
    lexer.print(stderr, currentLex);
#endif

    if (currentLex->type == SCR_LEX_ERROR) {
        throw ParserException(currentLex,
            getLine(), getPos(),
            __FILE__, __LINE__);
    }
}

bool Parser::tryLex(ScriptLexemeType type) const
{
    return (currentLex->type == type);
}

bool Parser::tryLex(ScriptLexemeType type, int aIntValue) const
{
    return (currentLex->type == type) &&
        (currentLex->intValue == aIntValue);
}

/* '+', '-', '!' */
bool Parser::isLexMonadicOp() const
{
    return tryLex(SCR_LEX_OPERATION, OP_PLUS) ||
        tryLex(SCR_LEX_OPERATION, OP_MINUS) ||
        tryLex(SCR_LEX_OPERATION, OP_NOT);
}

/* '*', '/', '%' */
bool Parser::isLexArithmeticOp_1() const
{
    return tryLex(SCR_LEX_OPERATION, OP_MULTIPLICATION) ||
        tryLex(SCR_LEX_OPERATION, OP_DIVISION) ||
        tryLex(SCR_LEX_OPERATION, OP_REMAINDER_DIVISION);
}

/* '+', '-' */
bool Parser::isLexArithmeticOp_2() const
{
    return tryLex(SCR_LEX_OPERATION, OP_PLUS) ||
        tryLex(SCR_LEX_OPERATION, OP_MINUS);
}

/* '<', '>', '==' */
bool Parser::isLexCompareOp() const
{
    return tryLex(SCR_LEX_OPERATION, OP_LESS) ||
        tryLex(SCR_LEX_OPERATION, OP_MORE) ||
        tryLex(SCR_LEX_OPERATION, OP_EQUALITY);
}

/* '&&' */
bool Parser::isLexLogicOp_1() const
{
    return tryLex(SCR_LEX_OPERATION, OP_AND);
}

/* '||' */
bool Parser::isLexLogicOp_2() const
{
    return tryLex(SCR_LEX_OPERATION, OP_OR);
}

PolizOpInt1Type Parser::getPolizOpInt1Type(int op) const
{
    switch (op) {
    case OP_PLUS:
        return POLIZ_OP_MONADIC_PLUS;
    case OP_MINUS:
        return POLIZ_OP_MONADIC_MINUS;
    case OP_NOT:
        return POLIZ_OP_LOGIC_NOT;
    }

    throw ParserException("Cannot get monadic int"
        "operation type; internal error.\n",
        getLine(), getPos(),
        __FILE__, __LINE__);
}

PolizOpInt2Type Parser::getPolizOpInt2Type(int op) const
{
    switch (op) {
    case OP_MULTIPLICATION:
        return POLIZ_OP_MULTIPLICATION;
    case OP_DIVISION:
        return POLIZ_OP_DIVISION;
    case OP_REMAINDER_DIVISION:
        return POLIZ_OP_REMAINDER_DIVISION;
    case OP_PLUS:
        return POLIZ_OP_PLUS;
    case OP_MINUS:
        return POLIZ_OP_MINUS;
    case OP_LESS:
        return POLIZ_OP_LESS;
    case OP_MORE:
        return POLIZ_OP_MORE;
    case OP_EQUALITY:
        return POLIZ_OP_EQUALITY;
    case OP_AND:
        return POLIZ_OP_LOGIC_AND;
    case OP_OR:
        return POLIZ_OP_LOGIC_OR;
    }

    throw ParserException("Cannot get two-arguments int"
        "operation type; internal error.\n",
        getLine(), getPos(),
        __FILE__, __LINE__);
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
        poliz.push(new PolizOpSet());
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
        poliz.push(new PolizOpPrint());
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
    int i = 1;

    while (tryLex(SCR_LEX_COMMA)) {
        getNextLex(); // skip ','
        PrintArg();
        ++i;
    }

    poliz.push(new PolizInt(i));
}

void Parser::PrintArg()
{
    if (tryLex(SCR_LEX_STRING)) {
        // TODO: use tables and PolizString(key)
        poliz.push(new PolizString(
            const_cast<char*>(currentLex->strValue)));
        getNextLex(); // skip 'string'
    } else {
        Expr();
    }
}

void Parser::Expr_0()
{
    if (tryLex(SCR_LEX_NUMBER)) {
        poliz.push(new PolizInt(currentLex->intValue));
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
    PolizElemList opList;

    while (isLexMonadicOp()) {
        opList.push(new PolizOpInt1(getPolizOpInt1Type(
            currentLex->intValue)));
        getNextLex();
    }

    Expr_0();

    while (!opList.isEmpty()) {
        poliz.push(opList.pop());
    }
}

void Parser::Expr_2()
{
    int op;

    Expr_1();

    while (isLexArithmeticOp_1()) {
        op = currentLex->intValue;
        getNextLex();
        Expr_1();
        poliz.push(new PolizOpInt2(getPolizOpInt2Type(op)));
    }
}

void Parser::Expr_3()
{
    int op;

    Expr_2();

    while (isLexArithmeticOp_2()) {
        op = currentLex->intValue;
        getNextLex();
        Expr_2();
        poliz.push(new PolizOpInt2(getPolizOpInt2Type(op)));
    }
}

void Parser::Expr_4()
{
    int op;

    Expr_3();

    while (isLexCompareOp()) {
        op = currentLex->intValue;
        getNextLex();
        Expr_3();
        poliz.push(new PolizOpInt2(getPolizOpInt2Type(op)));
    }
}

void Parser::Expr_5()
{
    int op;

    Expr_4();

    while (isLexLogicOp_1()) {
        op = currentLex->intValue;
        getNextLex();
        Expr_4();
        poliz.push(new PolizOpInt2(getPolizOpInt2Type(op)));
    }
}

void Parser::Expr()
{
    int op;

    Expr_5();

    while (isLexLogicOp_2()) {
        op = currentLex->intValue;
        getNextLex();
        Expr_5();
        poliz.push(new PolizOpInt2(getPolizOpInt2Type(op)));
    }
}

/* Argument def mean: "must be defined".
 * See README for more information. */
void Parser::Variable(bool def)
{
    if (! tryLex(SCR_LEX_VARIABLE)) {
        throw ParserException("Expected variable.",
            getLine(), getPos(),
            __FILE__, __LINE__);
    }

    int variableKey = -1;

    try {
        variableKey = tables.getVariableKey(currentLex->strValue, def);
    } catch(TableAccessException& ex) {
        throw ParserException(ex,
            getLine(), getPos(),
            __FILE__, __LINE__);
    }

    if (def) {
        poliz.push(new PolizOpVariableValue(variableKey));
    } else {
        poliz.push(new PolizVariable(variableKey));
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
    poliz(),
    tables()
        {}

void Parser::parse()
{
    getNextLex(); // get first lexeme

    Program();

    PolizElemList stack; // for constants
    poliz.evaluate(stack, tables);

    if (!stack.isEmpty()) {
        throw ParserException("Not empty stack"
            " after script interpretation.",
            getLine(), getPos(),
            __FILE__, __LINE__);
    }
}
