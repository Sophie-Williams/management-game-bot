#ifndef SCRIPT_LEXER_SENTRY
#define SCRIPT_LEXER_SENTRY

#if 0
#include <stdlib.h>
#include <ctype.h>
#endif
#include "String/String.hpp"
#include "CharQueue.hpp"

enum ScriptLexemeType {
    SCR_LEX_LABEL,    /* @[a-zA-Z0-9_]+ */
    /* variable or array */
    SCR_LEX_VARIABLE, /* $[a-zA-Z0-9_]+ */
    SCR_LEX_OPERATOR,
    /* internal function */
    SCR_LEX_FUNCTION, /* ?[a-zA-Z0-9_]+ */
    SCR_LEX_OPERATION,
    /* *, /, %, +, -, <, >, =, &&, ||, ! */
    SCR_LEX_BRACKET, /* (, ), [, ] */
    SCR_LEX_NUMBER,
    SCR_LEX_STRING,
    SCR_LEX_EOF
};

struct ServerMsg {
    ScriptLexemeType type;
    /* TODO */
};

#if 0
#ifndef INTERNAL_ERROR
#define INTERNAL_ERROR 1
#endif
#endif


class ScriptLexer {
    enum lexerState {
    };

    lexerState state;
    String tmpBuffer;
    ScriptLexemeType lexType;
    int requestNextChar;
    CharQueue queue;
    int c;

    void die(int line);
    ScriptLexemeType getLexType(const String &str) const;
    ScriptLexeme* stStart();

public:
    ScriptLexer();
    void putNewData(char *newBuffer, int size);
    ScriptLexeme* getLex();
};

#endif /* SCRIPT_LEXER_SENTRY */
