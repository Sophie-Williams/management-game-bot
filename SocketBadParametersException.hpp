#ifndef SOCKET_BAD_PARAMETERS_EXCEPTION_HPP_SENTRY
#define SOCKET_BAD_PARAMETERS_EXCEPTION_HPP_SENTRY

#include <stdio.h>
#include "Exception.hpp"
#include "String.hpp"

class SocketBadParametersException : public Exception {
    const char* badHost;
    const char* badPort;

public:
    SocketBadParametersException(const char* aBadHost,
        const char* aBadPort,
        const char* aFile, int aLine)
        : Exception(aFile, aLine),
        badHost(aBadHost),
        badPort(aBadPort)
            {}

    bool hostIsBad() const
    {
        return (badHost != 0);
    }

    bool portIsBad() const
    {
        return (badPort != 0);
    }


    const char* getBadHost() const
    {
        return badHost;
    }

    const char* getBadPort() const
    {
        return badPort;
    }

    const char* toString() const
    {
        String msg("SocketBadParametersException(");

        if (hostIsBad()) {
            msg += "host: \"";
            msg += badHost;
            msg += "\"";
        }

        if (portIsBad()) {
            msg += "port: \"";
            msg += badPort;
            msg += "\"";
        }

        msg += "): ";
        msg += getFile();
        msg += ":";

        char *lineStr = 0;
        asprintf(&lineStr, "%d", getLine());

        msg += lineStr;

        return msg.getCharPtr();
    }
};

#endif /* SOCKET_BAD_PARAMETERS_EXCEPTION_HPP_SENTRY */
