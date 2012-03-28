#ifndef CHAR_QUEUE_HPP_SENTRY
#define CHAR_QUEUE_HPP_SENTRY

#include <stdio.h>

class CharQueue {
    char *ptr;
    char *readPointer;
    int readAvailable;
    int onEOF;

public:
    CharQueue();
    void putNewData(char *buffer, int size)
        throw (int);
    void putEOF()
        throw (int);
    int hasNextChar();
    int getNextChar()
        throw (int);
};

#endif /* CHAR_QUEUE_HPP_SENTRY */
