#include "ScriptLexer.hpp"

void ScriptLexer::die(int line)
{
#ifndef DAEMON
    fprintf(stderr,
        "Sorry, I'm bad lexer. See %s:%d\n", __FILE__, line);
#endif
    exit(INTERNAL_ERROR);
}

ScriptLexeme *ScriptLexer::stStart()
{
    if (isSpaceSymbol(c))
        return;

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
        lexType = SRC_LEX_OPERATOR;
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
        tmpBuffer = "Unrecognized symbol: \'" + c + "\'";
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
        tmpBuffer = "Bad first identifier symbol: \'"
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
        /* TODO: save lexeme subtype (e.g. symbol) */
        return new ScriptLexeme(SCR_LEX_OPERATION);
        break;
    case '(':
    case ')':
    case '[':
    case ']':
        /* TODO: save lexeme subtype (e.g. symbol) */
        return new ScriptLexeme(SCR_LEX_BRACKET);
        break;
    case ':':
        return new ScriptLexeme(SCR_LEX_COLON);
        break;
    case ';':
        return new ScriptLexeme(SCR_LEX_SEMICOLON);
        break;
    case ',':
        return new ScriptLexeme(SCR_LEX_COMMA);
        break;
    }
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
    if (isTwoSymLexSymbol() && tmpBuffer.isEqual(c)) {
        tmpBuffer.clear();
        state = ST_START;
        /* TODO: save lexeme subtype (e.g. symbol) */
        return new ScriptLexeme(SCR_LEX_OPERATION);
    } else {
        notTakeNextChar = 1;
        tmpBuffer = "Lexer can not make two-symbol "
            "lexeme, bad symbol: \'" + c + "\'";
        state = ST_ERROR;
        return 0;
    }
}

ScriptLexeme* ScriptLexer::stNumber()
{
    if (idDigit(c)) {
        tmpBuffer += c;
        return 0;
    } else {
        tmpBuffer.clear();
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
        return ScriptLexeme(SCR_LEX_STRING);
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
        return new ScriptLexeme(SRC_LEX_EOF);
    } else {
        tmpBuffer = "Symbol after EOF: \'" + c "\'";
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

void ScriptLexer::ScriptLexer()
    : state(ST_START),
    tmpBuffer(),
    /* lexType is undefined */
    notTakeNextChar(0),
    queue()
    /* c is undefined */
        {}

ScriptLexeme* getLex()
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

        switch (state) {
        case ST_START:
            lex = stStart();
            break;
        } /* switch */
    } while (lex == NULL);

    /* Additional check */
    if (state != ST_START) {
        die(__LINE__);
    }

    return lex;
}
