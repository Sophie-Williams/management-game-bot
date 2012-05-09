#ifndef SERVER_MSG_LEXER_HPP_SENTRY
#define SERVER_MSG_LEXER_HPP_SENTRY

#include <stdlib.h>
#include <ctype.h>
#include "String.hpp"
#include "CharQueue.hpp"
#include "MsgStatus.hpp"

//TODO: make nice messages architecture.
#if 0
class ServerMessage {
public:
    virtual ~ServerMessage() {}
};

class ResponceMessage : public ServerMessage {
};

enum OkFailMessageType {
    MSG_NICK,
    MSG_BUILD,
    MSG_MAKE,
    MSG_BUY,
    MSG_SELL,
    MSG_TURN,
    MSG_JOIN
};

class OkFailMessage : public ResponceMessage {
    OkFailMessageType type;
    bool ok;

public:
    bool isOk() const
    {
        return ok;
    }

    bool hasType(OkFailMessageType aType) const
    {
        return (type == aType);
    }
};

class MarketStatusMessage : public ResponceMessage {
    int getMarketRaws() const
        { return values[2]; }

    int getMarketRawMinPrice() const
        { return values[3]; }

    int getMarketProductions() const
        { return values[4]; }

    int getMarketProdMaxPrice() const
        { return values[5]; }
};

class PlayerStatusMessage : public ResponceMessage {
    int getPlayerMoney() const
        { return values[0]; }

    int getPlayerRaws() const
        { return values[1]; }

    int getPlayerProductions() const
        { return values[2]; }

    int getPlayerFactories() const
        { return values[3]; }
};

class AsyncMessage : public ServerMessage {
};

class MonthCompletedMessage : public AsyncMessage {
};

class GameReadyMessage : public AsyncMessage {
};

class WinMessage : public AsyncMessage {
    const char* winners;

public:
    WinMessage(const char* aWinners)
        : winners(aWinners)
    {}

    const char* getWinners() const
    {
        return winners;
    }
};
#endif

enum TypeOfServerMsg {
    MSG_STATUS_RESPONCE,
    MSG_NICK_RESPONCE,
    MSG_BUILD_RESPONCE,
    MSG_MAKE_RESPONCE,
    MSG_BUY_RESPONCE,
    MSG_SELL_RESPONCE,
    MSG_TURN_RESPONCE,
    MSG_JOIN_RESPONCE,
    MSG_MONTH_COMPLETED,
    MSG_ROUNDS_ASYNC,
    MSG_WINNERS_ASYNC,
    MSG_UNKNOWN,
    MSG_LEXER_ERROR
};

struct ServerMsg {
    TypeOfServerMsg type;
    int ok;
    char *str;
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
