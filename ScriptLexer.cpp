#include "ScriptLexer.hpp"

char ScriptLexer::spaceSymbols[] = " \t\n"; /* and EOF */
char ScriptLexer::oneSymLexSymbols[] = "*/%+-<>!()[]:;,";
char ScriptLexer::twoSymLexSymbols[] = "=&|"; /* ==, &&, || */

void ScriptLexer::die(int line)
{
#ifndef DAEMON
    fprintf(stderr,
        "Sorry, I'm bad lexer. See %s:%d\n", __FILE__, line);
#endif
    exit(INTERNAL_ERROR);
}

int ScriptLexer::isSpaceSymbol(int c)
{
    char *p = spaceSymbols;

    while (*p != '\0') {
        if (*p == c)
            return 1;
        ++p;
    }

    return 0;
}

int ScriptLexer::isFirstIdentifierSymbol(int c)
{
    return ((c >= 'a') && (c <= 'z'))
        || ((c >= 'A') && (c <= 'Z'))
        || c == '_';
}

int ScriptLexer::isIdentifierSymbol(int c)
{
    return ((c >= '0') && (c <= '9'))
        || ((c >= 'a') && (c <= 'z'))
        || ((c >= 'A') && (c <= 'Z'))
        || c == '_';

}

int ScriptLexer::isOneSymLexSymbol(int c)
{
    char *p = oneSymLexSymbols;

    while (*p != '\0') {
        if (*p == c)
            return 1;
        ++p;
    }

    return 0;
}

int ScriptLexer::isTwoSymLexSymbol(int c)
{
    char *p = twoSymLexSymbols;

    while (*p != '\0') {
        if (*p == c)
            return 1;
        ++p;
    }

    return 0;
}

int ScriptLexer::isDigit(int c)
{
    return (c >= '0') && (c <= '9');
}

ScriptLexeme *ScriptLexer::stStart()
{
    if (isSpaceSymbol(c))
        return 0;

    if (c == '@') {
        lexType = SCR_LEX_LABEL;
        state = ST_IDENTIFIER_FIRST;
    } else if (c == '$') {
        lexType = SCR_LEX_VARIABLE;
        state = ST_IDENTIFIER_FIRST;
    } else if (c == '?') {
        lexType = SCR_LEX_FUNCTION;
        state = ST_IDENTIFIER_FIRST;
    } else if (isFirstIdentifierSymbol(c)) {
        notTakeNextChar = 1;
        lexType = SCR_LEX_OPERATOR;
        state = ST_IDENTIFIER_FIRST;
    } else if (isOneSymLexSymbol(c)) {
        notTakeNextChar = 1;
        state = ST_ONE_SYM_LEX;
    } else if (isTwoSymLexSymbol(c)) {
        notTakeNextChar = 1;
        state = ST_TWO_SYM_LEX_FIRST;
    } else if (isDigit(c)) {
        notTakeNextChar = 1;
        state = ST_NUMBER;
    } else if (c == '\"') {
        state = ST_STRING;
    } else if (c == EOF) {
        notTakeNextChar = 1;
        state = ST_EOF;
    } else {
        notTakeNextChar = 1;
        tmpBuffer = String("Unrecognized symbol: \'")
            + c + "\'";
        state = ST_ERROR;
    }

    return 0;
}

ScriptLexeme* ScriptLexer::stIdentifierFirst()
{
    if (isFirstIdentifierSymbol(c)) {
        tmpBuffer = c;
        state = ST_IDENTIFIER;
    } else {
        notTakeNextChar = 1;
        tmpBuffer = String("Bad first identifier symbol: \'")
            + c + "\'";
        state = ST_ERROR;
    }

    return 0;
}

ScriptLexeme* ScriptLexer::stIdentifier()
{
    if (isIdentifierSymbol(c)) {
        tmpBuffer += c;
        return 0;
    } else {
        tmpBuffer.clear();
        notTakeNextChar = 1;
        state = ST_START;
        return new ScriptLexeme(lexType);
    }
}

ScriptLexeme* ScriptLexer::stOneSymLex()
{
    /* We absolutelly sure that
     * isOneSymLexSymbol(c) is true. */

    /* Additional check */
    if (!isOneSymLexSymbol(c)) {
        die(__LINE__);
    }

    switch (c) {
    case '*':
    case '/':
    case '%':
    case '+':
    case '-':
    case '<':
    case '>':
    case '!':
        state = ST_START;
        /* TODO: save lexeme subtype (e.g. symbol) */
        return new ScriptLexeme(SCR_LEX_OPERATION);
    case '(':
    case ')':
    case '[':
    case ']':
        state = ST_START;
        /* TODO: save lexeme subtype (e.g. symbol) */
        return new ScriptLexeme(SCR_LEX_BRACKET);
    case ':':
        state = ST_START;
        return new ScriptLexeme(SCR_LEX_COLON);
    case ';':
        state = ST_START;
        return new ScriptLexeme(SCR_LEX_SEMICOLON);
    case ',':
        state = ST_START;
        return new ScriptLexeme(SCR_LEX_COMMA);
    }

    /* Not possible */
    return 0;
}

ScriptLexeme* ScriptLexer::stTwoSymLexFirst()
{
    /* We absolutelly sure that
     * isTwoSymLexSymbol(c) is true. */

    /* Additional check */
    if (!isTwoSymLexSymbol(c)) {
        die(__LINE__);
    }

    tmpBuffer = c;
    state = ST_TWO_SYM_LEX;
    return 0;
}

ScriptLexeme* ScriptLexer::stTwoSymLex()
{
    if (isTwoSymLexSymbol(c) && tmpBuffer.isEqual(c)) {
        tmpBuffer.clear();
        state = ST_START;
        /* TODO: save lexeme subtype (e.g. symbol) */
        return new ScriptLexeme(SCR_LEX_OPERATION);
    } else {
        notTakeNextChar = 1;
        tmpBuffer = String("Lexer can not make two-symbol "
            "lexeme, bad symbol: \'") + c + "\'";
        state = ST_ERROR;
        return 0;
    }
}

ScriptLexeme* ScriptLexer::stNumber()
{
    if (isDigit(c)) {
        tmpBuffer += c;
        return 0;
    } else {
        tmpBuffer.clear();
        notTakeNextChar = 1;
        state = ST_START;
        /* TODO: save number */
        return new ScriptLexeme(SCR_LEX_NUMBER);
    }
}

ScriptLexeme* ScriptLexer::stString()
{
    if (c == '\"') {
        tmpBuffer.clear();
        state = ST_START;
        /* TODO: save string */
        return new ScriptLexeme(SCR_LEX_STRING);
    } else if (c == EOF) {
        notTakeNextChar = 1;
        tmpBuffer = "Not terminated string: EOF";
        state = ST_ERROR;
        return 0;
    } else {
        tmpBuffer += c;
        return 0;
    }
}

ScriptLexeme* ScriptLexer::stEOF()
{
    notTakeNextChar = 1;

    if (c == EOF) {
        return new ScriptLexeme(SCR_LEX_EOF);
    } else {
        tmpBuffer = String("Symbol after EOF: \'") + c + "\'";
        state = ST_ERROR;
        return 0;
    }
}

ScriptLexeme* ScriptLexer::stError()
{
    notTakeNextChar = 1;
    /* TODO: save error message */
    tmpBuffer.clear();
    return new ScriptLexeme(SCR_LEX_ERROR);
}

ScriptLexeme* ScriptLexer::invokeStateHandler(LexerState state)
{
    switch (state) {
    case ST_START:
        return stStart();
    case ST_IDENTIFIER_FIRST:
        return stIdentifierFirst();
    case ST_IDENTIFIER:
        return stIdentifier();
    case ST_ONE_SYM_LEX:
        return stOneSymLex();
    case ST_TWO_SYM_LEX_FIRST:
        return stTwoSymLexFirst();
    case ST_TWO_SYM_LEX:
        return stTwoSymLex();
    case ST_NUMBER:
        return stNumber();
    case ST_STRING:
        return stString();
    case ST_EOF:
        return stEOF();
    case ST_ERROR:
        return stError();
    }

    /* Not possible */
    return 0;
}


ScriptLexer::ScriptLexer()
    : state(ST_START),
    tmpBuffer(),
    /* lexType is undefined */
    notTakeNextChar(0),
    queue()
    /* c is undefined */
        {}

void ScriptLexer::putNewData(char *buffer, int size)
{
    queue.putNewData(buffer, size);
}

void ScriptLexer::putEOF()
{
    queue.putEOF();
}

ScriptLexeme* ScriptLexer::getLex()
{
    ScriptLexeme *lex = NULL;

    do {
        /* Additional check */
        if (!notTakeNextChar &&
            (state == ST_EOF ||
            state == ST_ERROR))
        {
            die(__LINE__);
        }

        if (notTakeNextChar) {
            /* One-step delay */
            notTakeNextChar = 0;
        } else {
            if (queue.hasNextChar()) {
                c = queue.getNextChar();
            } else {
                /* Request for new data */
                return 0;
            }
        }

        lex = invokeStateHandler(state);
    } while (lex == 0);

    /* Additional check */
    if (state != ST_START &&
        state != ST_EOF &&
        state != ST_ERROR)
    {
        die(__LINE__);
    }

    return lex;
}
