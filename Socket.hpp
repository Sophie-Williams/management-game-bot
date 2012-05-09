#ifndef SOCKET_HPP_SENTRY
#define SOCKET_HPP_SENTRY

#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include "SocketIOException.hpp"
#include "SocketBadParametersException.hpp"

class Socket {
    in_addr host; // network byte order
    uint16_t port; // network byte order
    int fd; // socket file descriptor

    int socketSyscallWrapper()
        throw (SocketIOException);

    void connectSyscallWrapper(int sockfd,
        const in_addr& host, uint16_t port)
        throw (SocketIOException);

    void shutdownSyscallWrapper(int sockfd, int how)
        throw (SocketIOException);

    void closeSyscallWrapper(int sockfd)
        throw (SocketIOException);

public:
    Socket(const char* host, const char* port)
        throw (SocketBadParametersException);

    ~Socket()
        throw ();

    void connect()
        throw (SocketIOException);

    void disconnect()
        throw (SocketIOException);

    bool isReadAvailable()
        throw (SocketIOException);

    /* Blocking method. */
    /* Returns true if not EOF. */
    bool read(char *buf, size_t count,
        int* readed)
        throw (SocketIOException);

    void write(const char *buf, size_t count)
        throw (SocketIOException);

    bool isConnected() const
        throw ();
};

#endif /* SOCKET_HPP_SENTRY */
