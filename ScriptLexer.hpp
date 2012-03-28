#ifndef SCRIPT_LEXER_SENTRY
#define SCRIPT_LEXER_SENTRY

#if 0
#include <stdlib.h>
#include <ctype.h>
#endif
#include "String/String.hpp"
#include "CharQueue.hpp"

enum ScriptLexemeType {
    SCR_LEX_LABEL,
    /* @[a-zA-Z_][a-zA-Z0-9_]* */
    SCR_LEX_VARIABLE,
    /* variable or array */
    /* $[a-zA-Z_][a-zA-Z0-9_]* */
    SCR_LEX_FUNCTION,
    /* internal function */
    /* ?[a-zA-Z_][a-zA-Z0-9_]* */
    SCR_LEX_OPERATOR,
    /* [a-zA-Z_][a-zA-Z0-9_]* */
    SCR_LEX_OPERATION,
    /* *, /, %, +, -, <, >, ==, &&, ||, ! */
    SCR_LEX_BRACKET,   /* (, ), [, ] */
    SCR_LEX_COLON,     /* : */
    SCR_LEX_SEMICOLON, /* ; */
    SCR_LEX_COMMA,     /* , */
    SCR_LEX_NUMBER,
    SCR_LEX_STRING,
    SCR_LEX_EOF
};

struct ScriptLexeme {
    ScriptLexemeType type;
    /* TODO */

    ScriptLexeme(ScriptLexemeType aType)
        : type(aType) {}
};

#ifndef INTERNAL_ERROR
#define INTERNAL_ERROR 1
#endif


class ScriptLexer {
    static char spaceSymbols[] = " \t\n"; /* and EOF */
    static char oneSymLexSymbols[] = "*/%+-<>!()[]:;,";
    static char twoSymLexSymbols[] = "=&|"; /* ==, &&, || */

    enum lexerState {
        ST_START,
        ST_IDENTIFIER_FIRST,
        ST_IDENTIFIER,
        ST_ONE_SYM_LEX,
        ST_TWO_SYM_LEX_FIRST,
        ST_TWO_SYM_LEX,
        ST_NUMBER,
        ST_STRING,
        ST_EOF,
        ST_ERROR
    };

    lexerState state;
    String tmpBuffer;
    ScriptLexemeType lexType;
    int notTakeNextChar;
    CharQueue queue;
    int c;

    void die(int line);

    /* TODO: move to special class */
    int isSpaceSymbol(int c);
    int isFirstIdentifierSymbol(int c);
    int isOneSymLexSymbol(int c);
    int isTwoSymLexSymbol(int c);
    int isDigit(int c);
    // TODO: necessary?
    //int isDelimiter(int c); /* space or one/two sym lex */

    ScriptLexeme* stStart();
    ScriptLexeme* stIdentifierFirst();
    ScriptLexeme* stIdentifier();
    ScriptLexeme* stOneSymLex();
    ScriptLexeme* stTwoSymLexFirst();
    ScriptLexeme* stTwoSymLex();
    ScriptLexeme* stNumber();
    ScriptLexeme* stString();
    ScriptLexeme* stEOF();
    ScriptLexeme* stError();

public:
    ScriptLexer();
    void putNewData(char *newBuffer, int size);
    void putEOF();
    ScriptLexeme* getLex();
};

#endif /* SCRIPT_LEXER_SENTRY */
