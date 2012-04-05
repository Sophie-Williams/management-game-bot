#ifndef SERVER_MSG_LEXER_HPP_SENTRY
#define SERVER_MSG_LEXER_HPP_SENTRY

#include <stdlib.h>
#include <ctype.h>
#include "String.hpp"
#include "CharQueue.hpp"

enum TypeOfServerMsg {
    MSG_STATUS_RESPONCE,
    MSG_NICK_RESPONCE,
    MSG_BUILD_RESPONCE,
    MSG_MAKE_RESPONCE,
    MSG_BUY_RESPONCE,
    MSG_SELL_RESPONCE,
    MSG_TURN_RESPONCE,
    MSG_JOIN_RESPONCE,
    MSG_ROUNDS_ASYNC,
    MSG_UNKNOWN,
    MSG_LEXER_ERROR
};

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

struct ServerMsg {
    TypeOfServerMsg type;
    int ok;
    MsgStatus* status;

#ifndef DAEMON
    void print(FILE *stream);
#endif
};

#ifndef INTERNAL_ERROR
#define INTERNAL_ERROR 1
#endif

class ServerMsgLexer {
    static const char* typeOfServerMsgStr[];

    enum lexerState {
        ST_START,
        ST_HEAD_START,
        ST_HEAD_SKIP_TIMESTAMP,
        ST_SKIP_TO_NEXT_HEAD,
        ST_OK_FAIL_RESPONCE,
        ST_STATUS_RESPONCE,
        ST_STATUS_READ_VALUE,
        ST_STATUS_SKIP_TO_NEWLINE,
        ST_ASYNC_MSG,
        ST_ERROR
    };

    lexerState state;
    int timestampSkipped;
    String tmpBuffer;
    TypeOfServerMsg msgType;
    int requestNextChar;
    CharQueue queue;
    int c;

    /* Temporally values */
    int tmpValue;
    MsgStatus* tmpStatus;

    void die(int line);
    TypeOfServerMsg getMsgType(const String &str) const;
    ServerMsg* stStart();
    ServerMsg* stHeadStart();
    ServerMsg* stHeadSkipTimestamp();
    ServerMsg* stSkipToNextHead();
    ServerMsg* stOkFailResponce();
    ServerMsg* stAsyncMsg();
    ServerMsg* stStatusResponce();
    ServerMsg* stStatusReadValue();
    ServerMsg* stStatusSkipToNewline();
    ServerMsg* stError();

public:
    ServerMsgLexer();
    void putNewData(char *buffer, int size);
    ServerMsg* getMsg();
};

#endif /* SERVER_MSG_LEXER_HPP_SENTRY */
