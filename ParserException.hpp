#ifndef PARSER_EXCEPTION_HPP_SENTRY
#define PARSER_EXCEPTION_HPP_SENTRY

#include "Exception.hpp"
#include "String.hpp"
#include "ScriptLexer.hpp"
#include "CharQueueException.hpp"

class ParserException : public Exception {
    int scriptLine;
    int scriptPos;
    const char* description;

public:
    ParserException(const char* aDescription,
        int aScriptLine, int aScriptPos,
        const char* aFile, int aLine)
        : Exception(aFile, aLine),
        scriptLine(aScriptLine),
        scriptPos(aScriptPos),
        description(aDescription)
            {}

    ParserException(const ScriptLexeme* errorLex,
        int aScriptLine, int aScriptPos,
        const char* aFile, int aLine)
        : Exception(aFile, aLine),
        scriptLine(aScriptLine),
        scriptPos(aScriptPos)
    {
        /* errorLex->type == SCR_LEX_ERROR */
        String str = "Error in lexer. ";
        str += errorLex->strValue;
        str += " Symbol: '";
        if (errorLex->intValue == EOF)
            str += "EOF";
        else
            str += static_cast<char>(errorLex->intValue);
        str += "'.";
        description = str.getCharPtr();
    }

    ParserException(const CharQueueException& ex,
        int aScriptLine, int aScriptPos,
        const char* aFile, int aLine)
        : Exception(aFile, aLine),
        scriptLine(aScriptLine),
        scriptPos(aScriptPos)
    {
        description = ex.toString();
    }

    const char* getDescription() const
    {
        return description;
    }

    const char* toString() const
    {
        String msg("ParserExpection(\"");

        msg += description;
        msg += "\"); ";
        msg += getFile();

        char *numbers = 0;
        asprintf(&numbers, ":%d; Script: %d:%d\n",
            getLine(), scriptLine, scriptPos);

        msg += numbers;

        return msg.getCharPtr();
    }
};

#endif /* PARSER_EXCEPTION_HPP_SENTRY */
