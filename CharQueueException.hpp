#ifndef CHAR_QUEUE_EXCEPTION_HPP_SENTRY
#define CHAR_QUEUE_EXCEPTION_HPP_SENTRY

#include "Exception.hpp"
#include "String.hpp"

class CharQueueException : public Exception {
    const char* description;

public:
    CharQueueException(const char* aDescription,
        const char* aFile, int aLine)
        : Exception(aFile, aLine),
        description(aDescription)
            {}

    const char* getDescription() const
    {
        return description;
    }

    const char* toString() const
    {
        String msg("CharQueueExpection(\"");

        msg += description;
        msg += "\"); ";
        msg += getFile();

        char *strLine = 0;
        asprintf(&strLine, ":%d", getLine());

        msg += strLine;

        return msg.getCharPtr();
    }
};

#endif /* PARSER_EXCEPTION_HPP_SENTRY */
