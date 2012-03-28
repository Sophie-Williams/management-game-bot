#include "CharQueue.hpp"

CharQueue::CharQueue()
        : readPointer(0),
        readAvailable(0),
        onEOF(0) {}

void CharQueue::putNewData(char *buffer, int size)
    throw (int)
{
    if (onEOF || readAvailable > 0)
        throw 1; /* TODO */

    readPointer = ptr = buffer;
    readAvailable = size;
}

void CharQueue::putEOF()
    throw (int)
{
    if (onEOF || readAvailable > 0)
        throw 1; /* TODO */

    onEOF = 1;
}

int CharQueue::hasNextChar()
{
    return (onEOF || readAvailable > 0);
}

char CharQueue::getNextChar()
    throw (int)
{
    int c;

    if (!onEOF && readAvailable <= 0)
        throw 2; /* TODO */

    if (onEOF) {
        onEOF = 0;
        return EOF;
    }

    c = *readPointer;
    --readAvailable;
    ++readPointer;

    return c;
}
