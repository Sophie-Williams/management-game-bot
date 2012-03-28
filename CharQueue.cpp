#include "CharQueue.hpp"

CharQueue::CharQueue()
        : readPointer(0),
        readAvailable(0) {}

void CharQueue::putNewData(char *buffer, int size)
    throw (int)
{
    if (readAvailable != 0)
        throw 1; /* TODO */

    readPointer = ptr = buffer;
    readAvailable = size;
}

int CharQueue::hasNextChar()
{
    return (readAvailable != 0);
}

char CharQueue::getNextChar()
    throw (int)
{
    int c;

    if (readAvailable <= 0)
        throw 2; /* TODO */

    c = *readPointer;

    --readAvailable;
    ++readPointer;

    return c;
}
