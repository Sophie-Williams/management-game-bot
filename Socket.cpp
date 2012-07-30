#include "Socket.hpp"

int Socket::socketSyscallWrapper()
    throw (SocketIOException)
{
    int socketValue = socket(AF_INET, SOCK_STREAM, 0);

    if (SocketIOException::isSocketError(socketValue)) {
        fd = -1; // not connected state
        throw SocketIOException("socket", __FILE__, __LINE__);
    }

    return socketValue;
}

void Socket::connectSyscallWrapper(int sockfd,
    const in_addr& host, uint16_t port)
    throw (SocketIOException)
{
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = port;
    addr.sin_addr = host;

    int connectValue = ::connect(sockfd,
        (sockaddr *) &addr, /* TODO: reinterpret_cast? */
        sizeof(addr));

    if (SocketIOException::isConnectError(connectValue)) {
        fd = -1; // not connected state
        throw SocketIOException("connect", __FILE__, __LINE__);
    }
}

void Socket::shutdownSyscallWrapper(int sockfd, int how)
    throw (SocketIOException)
{
    int shutdownValue = shutdown(sockfd, how);

    if (SocketIOException::isShutdownError(shutdownValue)) {
        fd = -1; // not connected state
        throw SocketIOException("shutdown", __FILE__, __LINE__);
    }
}

void Socket::closeSyscallWrapper(int sockfd)
    throw (SocketIOException)
{
    int closeValue = close(sockfd);

    if (SocketIOException::isCloseError(closeValue)) {
        fd = -1; // not connected state
        throw SocketIOException("close", __FILE__, __LINE__);
    }
}

Socket::Socket(const char* aHost, const char* aPort)
    throw (SocketBadParametersException)
    : fd(-1)
{
    int hostIsBad = (inet_aton(aHost, &host) == 0);
    int portTmpValue = atoi(aPort);

    if (hostIsBad && !(portTmpValue == 0)) {
        /* (fd == -1), ok */
        throw SocketBadParametersException(aHost, 0,
            __FILE__, __LINE__);
    } else if (!hostIsBad && (portTmpValue == 0)) {
        /* (fd == -1), ok */
        throw SocketBadParametersException(0, aPort,
            __FILE__, __LINE__);
    } else if (hostIsBad && (portTmpValue == 0)) {
        /* (fd == -1), ok */
        throw SocketBadParametersException(aHost, aPort,
            __FILE__, __LINE__);
    }

    port = htons(portTmpValue);
}

Socket::~Socket()
    throw ()
{}

void Socket::connect()
    throw (SocketIOException)
{
    fd = socketSyscallWrapper();
    connectSyscallWrapper(fd, host, port);
}

void Socket::disconnect()
    throw (SocketIOException)
{
    shutdownSyscallWrapper(fd, SHUT_RDWR);
    closeSyscallWrapper(fd);
    fd = -1;
}

bool Socket::isReadAvailable()
    throw (SocketIOException)
{
    fd_set set;
    FD_ZERO(&set);
    FD_SET(fd, &set);

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    int selectValue = select(fd + 1, &set, NULL, NULL, &tv);

    if (SocketIOException::isSelectError(selectValue)) {
        fd = -1; // not connected state
        throw SocketIOException("select", __FILE__, __LINE__);
    }

    return FD_ISSET(fd, &set);
}

bool Socket::read(char *buf, size_t count, int* readed)
    throw (SocketIOException)
{
    ssize_t readValue = ::read(fd, buf, sizeof(char) * count);
    /* TODO: necessary static_cast<void*>(buf) ? */

    if (SocketIOException::isReadError(readValue)) {
        fd = -1; // not connected state
        throw SocketIOException("read", __FILE__, __LINE__);
    }

    /* TODO: close (one direction of) socket if 0 */

    *readed = readValue;
    return (readValue != 0);
}

void Socket::write(const char *buf, size_t count)
    throw (SocketIOException)
{
    ssize_t writeValue = ::write(fd, buf, sizeof(char) * count);
    /* TODO: necessary static_cast<void*>(buf) ? */

    if (SocketIOException::isWriteError(writeValue)) {
        fd = -1; // not connected state
        throw SocketIOException("write", __FILE__, __LINE__);
    }
}

bool Socket::isConnected() const
    throw ()
{
    return (fd != -1);
}
