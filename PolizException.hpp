#ifndef POLIZ_EXCEPTION_HPP_SENTRY
#define POLIZ_EXCEPTION_HPP_SENTRY

#include "Exception.hpp"
#include "String.hpp"

class PolizException : public Exception {
    const char* description;

public:
    PolizException(const char* aDescription,
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
        String msg("PolizExpection(\"");

        msg += description;
        msg += "\"); ";
        msg += getFile();

        char *lineStr = 0;

        asprintf(&lineStr, ":%d", getLine());

        msg += lineStr;

        return msg.getCharPtr();
    }
};

#endif /* POLIZ_EXCEPTION_HPP_SENTRY */
