#ifndef INDEX_OUT_OF_BOUND_EXCEPTION_HPP_SENTRY
#define INDEX_OUT_OF_BOUND_EXCEPTION_HPP_SENTRY

#include <stdio.h>

/* TODO: extends Exception */
class IndexOutOfBoundException {
    int reqIndex;
    int maxIndex;

public:
    IndexOutOfBoundException(int req, int max)
        : reqIndex(req), maxIndex(max) {}

    int getReqIndex()
    {
        return reqIndex;
    }

    int getMaxIndex()
    {
        return maxIndex;
    }

    void print(FILE *stream)
    {
        fprintf(stream, "IndexOutOfBoundException: req %d; max: %d\n",
            reqIndex, maxIndex);
    }
};


#endif // INDEX_OUT_OF_BOUND_EXCEPTION_HPP_SENTRY
