#ifndef CHAR_QUEUE_HPP_SENTRY
#define CHAR_QUEUE_HPP_SENTRY

/* TODO: think about EOF in queue.
 * It can be implement by one boolean variable in class. */

class CharQueue {
    char *ptr;
    char *readPointer;
    int readAvailable;

public:
    CharQueue();
    void putNewData(char *buffer, int size)
        throw (int);
    int hasNextChar();
    char getNextChar()
        throw (int);
};

#endif /* CHAR_QUEUE_HPP_SENTRY */
