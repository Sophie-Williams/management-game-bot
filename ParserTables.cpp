#include "ParserTables.hpp"

const char* ParserTables::keywordStrings[] = {
    "set", "goto", "if", "else",
    "while", "print", "buy", "sell",
    "make", "build", "turn", ""
};

//ParserTables::ParserTables() {}

ScriptLangKeywords ParserTables::getKeywordType(const char* str) const
{
    for (unsigned int i = 0;
        i < sizeof(keywordStrings) / sizeof(char*); ++i)
    {
        if (STR_EQUAL(str, keywordStrings[i]))
            return (ScriptLangKeywords) i;
    }

    return SCR_KEYWORD_UNKNOWN;
}

