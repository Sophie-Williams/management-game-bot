#include "CharQueue.hpp"

CharQueue::CharQueue()
        : ptr(0),
        readPointer(0),
        readAvailable(0),
        onEOF(0) {}

void CharQueue::putNewData(char *buffer, int size)
    throw (CharQueueException)
{
    if (onEOF || readAvailable > 0) {
        throw CharQueueException("Cannot put new data.",
            __FILE__, __LINE__);
    }

    readPointer = ptr = buffer;
    readAvailable = size;
}

void CharQueue::putEOF()
    throw (CharQueueException)
{
    if (onEOF || readAvailable > 0) {
        throw CharQueueException("Cannot put EOF.",
            __FILE__, __LINE__);
    }

    onEOF = 1;
}

int CharQueue::hasNextChar()
{
    return (onEOF || readAvailable > 0);
}

int CharQueue::getNextChar()
    throw (CharQueueException)
{
    int c;

    if (!onEOF && readAvailable <= 0) {
        throw CharQueueException("Cannot get next symbol.",
            __FILE__, __LINE__);
    }

    if (onEOF) {
        onEOF = 0;
        return EOF;
    }

    c = *readPointer;
    --readAvailable;
    ++readPointer;

    return c;
}
