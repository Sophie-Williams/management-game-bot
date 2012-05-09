#ifndef GAME_ACTIONS_EXCEPTION_HPP_SENTRY
#define GAME_ACTIONS_EXCEPTION_HPP_SENTRY

#include "Exception.hpp"
#include "String.hpp"

class GameActionsException : public Exception {
    const char* description;

public:
    GameActionsException(const char* aDescription,
        const char* aFile, int aLine)
        : Exception(aFile, aLine),
        description(aDescription)
    {}

    const char* toString() const
    {
        String msg("GameAccessException(\"");

        msg += description;
        msg += "\"); ";
        msg += getFile();

        char *lineStr = 0;

        asprintf(&lineStr, ":%d\n", getLine());
        msg += lineStr;

        return msg.getCharPtr();
    }
};

#endif /* GAME_ACTIONS_EXCEPTION_HPP_SENTRY */
