#ifndef SOCKET_IO_EXCEPTION_HPP_SENTRY
#define SOCKET_IO_EXCEPTION_HPP_SENTRY

#include "Exception.hpp"
#include "String.hpp"

class SocketIOException : public Exception {
    const char* syscallName;

public:
    SocketIOException(const char* aSyscallName,
        const char* aFile, int aLine)
        : Exception(aFile, aLine),
        syscallName(aSyscallName)
            {}

    const char* getSyscallName() const
    {
        return syscallName;
    }

    const char* toString() const
    {
        String msg("SocketIOExpection(\"");

        msg += syscallName;
        msg += "\"): ";
        msg += getFile();
        msg += ":";

        char *lineStr = 0;
        asprintf(&lineStr, "%d", getLine());

        msg += lineStr;
        msg += "\n";

        return msg.getCharPtr();
    }

    static bool isSocketError(int value)
    {
        return (value == -1);
    }

    static bool isConnectError(int value)
    {
        return (value != 0);
    }

    static bool isShutdownError(int value)
    {
        return (value == -1);
    }

    static bool isCloseError(int value)
    {
        return (value == -1);
    }

    static bool isReadError(int value)
    {
        return (value == -1);
    }

    static bool isWriteError(int value)
    {
        return (value == -1);
    }
};

#endif /* SOCKET_IO_EXCEPTION_HPP_SENTRY */
