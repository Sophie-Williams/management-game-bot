#ifndef MSG_STATUS_HPP_SENTRY
#define MSG_STATUS_HPP_SENTRY

#ifndef DAEMON
#include <stdio.h>
#endif

class MsgStatus {
    int *values;
    int size;

public:
    MsgStatus()
        : values(0),
        size(0) {}

    void add(int number)
    {
        int *newValues = new int[size + 1];
        if (values) {
            memcpy(newValues, values,
                size * sizeof(int));
            delete[] values;
        }
        newValues[size] = number;
        values = newValues;
        ++size;
    }

#if 0
    int getSize() const
        { return size; }
#endif

#ifndef DAEMON
    void printValues(FILE *stream)
    {
        if (size == 0) {
            fprintf(stream, "empty\n");
            return;
        }

        for (int i = 0; i < (size - 1); ++i)
            fprintf(stream, "%d ", values[i]);
        fprintf(stream, "%d\n", values[size - 1]);
    }
#endif /* DAEMON */

    /* status --market */
    int getMarketRaws()
        { return values[2]; }

    /* status --market */
    int getMarketRawMinPrice()
        { return values[3]; }

    /* status --market */
    int getMarketProductions()
        { return values[4]; }

    /* status --market */
    int getMarketProdMaxPrice()
        { return values[5]; }

    /* status [username] */
    int getPlayerMoney()
        { return values[0]; }

    /* status [username] */
    int getPlayerRaws()
        { return values[1]; }

    /* status [username] */
    int getPlayerProductions()
        { return values[2]; }

    /* status [username] */
    int getPlayerFactories()
        { return values[3]; }
};

#endif /* MSG_STATUS_HPP_SENTRY */
