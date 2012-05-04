#ifndef TABLE_ITEM_NOT_FOUND_EXCEPTION_HPP_SENTRY
#define TABLE_ITEM_NOT_FOUND_EXCEPTION_HPP_SENTRY

#include "Exception.hpp"

class TableItemNotFoundException : public Exception {
    const char* description;
    int intKey;
    const char* strKey;

public:
    TableItemNotFoundException(const char* aDescription,
        int aIntKey,
        const char* aFile, int aLine)
        : Exception(aFile, aLine),
        description(aDescription),
        intKey(aIntKey),
        strKey(0)
    {}

    TableItemNotFoundException(const char* aDescription,
        const char* aStrKey,
        const char* aFile, int aLine)
        : Exception(aFile, aLine),
        description(aDescription),
        intKey(0),
        strKey(aStrKey)
    {}


    const char* getDescription() const
    {
        return description;
    }

    int getIntKey() const
    {
        return intKey;
    }

    const char* getStrKey() const
    {
        return strKey;
    }

    const char* toString() const
    {
        String msg("TableItemNotFoundException(\"");

        msg += description;
        msg += "\"); ";
        msg += getFile();

        char *numbers = 0;
        if (strKey == 0) {
            asprintf(&numbers, ":%d; Key: %d",
                getLine(), intKey);
        } else {
            asprintf(&numbers, ":%d; Key: %s",
                getLine(), strKey);
        }

        msg += numbers;

        return msg.getCharPtr();
    }
};

#endif /* TABLE_ITEM_NOT_FOUND_EXCEPTION_HPP_SENTRY */
