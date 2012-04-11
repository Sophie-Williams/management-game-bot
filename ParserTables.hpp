#ifndef PARSER_TABLES_HPP_SENTRY
#define PARSER_TABLES_HPP_SENTRY

#include "String.hpp"

enum ScriptLangKeywords {
    SCR_KEYWORD_SET,
    SCR_KEYWORD_GOTO,
    SCR_KEYWORD_IF,
    SCR_KEYWORD_ELSE,
    SCR_KEYWORD_WHILE,
    SCR_KEYWORD_PRINT,
    SCR_KEYWORD_BUY,
    SCR_KEYWORD_SELL,
    SCR_KEYWORD_MAKE,
    SCR_KEYWORD_BUILD,
    SCR_KEYWORD_TURN,
    SCR_KEYWORD_UNKNOWN
};

class ParserTables {
    static const char* keywordStrings[];

public:
//    ParserTables();
    ScriptLangKeywords getKeywordType(const char* str) const;
};

#endif /* PARSER_TABLES_HPP_SENTRY */
