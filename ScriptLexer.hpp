#ifndef SCRIPT_LEXER_SENTRY
#define SCRIPT_LEXER_SENTRY

#include <stdlib.h>
#ifndef DAEMON
#include <stdio.h>
#endif
#include "String.hpp"
#include "CharQueue.hpp"

// TODO: 65a must not '65' 'a'

enum Operations {
    OP_MULTIPLICATION,
    OP_DIVISION,
    OP_REMAINDER_DIVISION,
    OP_PLUS,
    OP_MINUS,
    OP_LESS,
    OP_MORE,
    OP_EQUALITY,
    OP_AND,
    OP_OR,
    OP_NOT,
    OP_UNKNOWN
};

enum Brackets {
    BRACKET_PARENTHESIS_OPEN,
    BRACKET_PARENTHESIS_CLOSE,
    BRACKET_SQUARE_OPEN,
    BRACKET_SQUARE_CLOSE,
    BRACKET_FIGURE_OPEN,
    BRACKET_FIGURE_CLOSE,
    BRACKET_UNKNOWN
};

enum ScriptLexemeType {
    SCR_LEX_LABEL,
    /* @[a-zA-Z_][a-zA-Z0-9_]* */
    SCR_LEX_VARIABLE,
    /* variable or array */
    /* $[a-zA-Z_][a-zA-Z0-9_]* */
    SCR_LEX_FUNCTION,
    /* internal function */
    /* ?[a-zA-Z_][a-zA-Z0-9_]* */
    SCR_LEX_KEYWORD,
    /* keyword */
    /* [a-zA-Z_][a-zA-Z0-9_]* */
    SCR_LEX_OPERATION,
    /* *, /, %, +, -, <, >, ==, &&, ||, ! */
    SCR_LEX_BRACKET,   /* (, ), [, ], {, } */
    SCR_LEX_COLON,     /* : */
    SCR_LEX_SEMICOLON, /* ; */
    SCR_LEX_COMMA,     /* , */
    SCR_LEX_NUMBER,
    SCR_LEX_STRING,
    SCR_LEX_EOF,
    SCR_LEX_ERROR
};

struct ScriptLexeme {
    ScriptLexemeType type;
    int intValue;
    const char *strValue;
    int line;
    int pos;

    ScriptLexeme(ScriptLexemeType aType)
        : type(aType), intValue(0), strValue(0)
        /* line and pos is undefined */
            {}

    ScriptLexeme(ScriptLexemeType aType, int aIntValue)
        : type(aType), intValue(aIntValue), strValue(0)
        /* line and pos is undefined */
            {}

    ScriptLexeme(ScriptLexemeType aType, const char* aStrValue)
        : type(aType), intValue(0), strValue(aStrValue)
        /* line and pos is undefined */
            {}

    ScriptLexeme(ScriptLexemeType aType, int aIntValue,
        const char* aStrValue)
        : type(aType), intValue(aIntValue), strValue(aStrValue)
        /* line and pos is undefined */
            {}

    ~ScriptLexeme()
    {
        if (strValue)
            delete[] strValue;
    }
};

#ifndef INTERNAL_ERROR
#define INTERNAL_ERROR 1
#endif

class ScriptLexer {
    static const char spaceSymbols[];
    static const char oneSymLexSymbols[];
    static const char twoSymLexSymbols[];

    static const char operationSymbols[];
    static const char bracketSymbols[];

    static const char* operationStrings[];
    static const char* bracketStrings[];

    enum LexerState {
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

    LexerState state;
    String tmpBuffer;
    ScriptLexemeType lexType;
    int notTakeNextChar;
    CharQueue queue;
    int c;

    int line;
    int pos;

    void die(int line);
    void updateLinePos(int& line, int& pos);

    int isSpaceSymbol(int c);
    int isFirstIdentifierSymbol(int c);
    int isIdentifierSymbol(int c);
    int isOneSymLexSymbol(int c);
    int isTwoSymLexSymbol(int c);
    int isDigit(int c);
//TODO:    int isDelimeter(int c);

    Operations getOperationType(int c);
    Brackets getBracketType(int c);
    const char *getOperationString(unsigned int idx);
    const char *getBracketString(unsigned int idx);

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

    ScriptLexeme* invokeStateHandler(LexerState state);
public:
    ScriptLexer();
    void putNewData(char *buffer, int size);
    void putEOF();
    ScriptLexeme* getLex();
#ifndef DAEMON
    void print(FILE *stream, const ScriptLexeme *lex);
#endif
};

#endif /* SCRIPT_LEXER_SENTRY */
