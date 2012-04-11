#ifndef PARSER_EXCEPTION_HPP_SENTRY
#define PARSER_EXCEPTION_HPP_SENTRY

#include "Exception.hpp"
#include "String.hpp"

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

    const char* getDescription() const
    {
        return description;
    }

    const char* toString() const
    {
        String msg("ParserExpection(\"");

        msg += description;
        msg += "\"): ";
        msg += getFile();

        char *numbers = 0;
        asprintf(&numbers, ":%d; in script: %d:%d\n",
            getLine(), scriptLine, scriptPos);

        msg += numbers;

        return msg.getCharPtr();
    }
};

#endif /* PARSER_EXCEPTION_HPP_SENTRY */
