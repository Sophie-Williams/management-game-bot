#ifndef CHAR_QUEUE_HPP_SENTRY
#define CHAR_QUEUE_HPP_SENTRY

#include <stdio.h>
#include "CharQueueException.hpp"

class CharQueue {
    char *ptr;
    char *readPointer;
    int readAvailable;
    int onEOF;

public:
    CharQueue();
    void putNewData(char *buffer, int size)
        throw (CharQueueException);
    void putEOF()
        throw (CharQueueException);
    int hasNextChar();
    int getNextChar()
        throw (CharQueueException);
};

#endif /* CHAR_QUEUE_HPP_SENTRY */
