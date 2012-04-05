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
    MSG_UNKNOWN,
    MSG_LEXER_ERROR
};

#if 0
struct PlayerInfo {
    int money;
    int raws;
    int productions;
    int factories;
};

struct MarketInfo {
    int raw;
    int raw_min_price;
    int prod;
    int prod_max_price;
}
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
        if (values)
            memcpy(newValues, values,
                size * sizeof(int));
        newValues[size] = number;
        ++size;
    }

    int get(int index) const
    {
        return values[index];
    }

    int getSize() const
    {
        return size;
    }
};

struct ServerMsg {
    TypeOfServerMsg type;
    int okResponce;
    MsgStatus* status;

    void print();
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
        ST_ERROR
    };

    lexerState state;
    int timestampSkipped;
    String tmpBuffer;
    TypeOfServerMsg msgType;
    /* TODO: int async; */
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
