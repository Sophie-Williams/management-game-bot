#ifndef SERVER_MSG_LEXER_HPP_SENTRY
#define SERVER_MSG_LEXER_HPP_SENTRY

#include <stdlib.h>
#include <ctype.h>
#include "String.hpp"
#include "CharQueue.hpp"
#include "MsgStatus.hpp"

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
