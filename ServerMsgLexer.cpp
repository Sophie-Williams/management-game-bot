#include "ServerMsgLexer.hpp"

void ServerMsg::print()
{
    switch (type) {
    case MSG_STATUS_RESPONCE:
        printf("MSG_STATUS_RESPONCE: %s\n",
            okResponce ? "ok" : "fail");
        break;
    case MSG_NICK_RESPONCE:
        printf("MSG_NICK_RESPONCE: %s\n",
            okResponce ? "ok" : "fail");
        break;
    case MSG_BUILD_RESPONCE:
        printf("MSG_BUILD_RESPONCE: %s\n",
            okResponce ? "ok" : "fail");
        break;
    case MSG_MAKE_RESPONCE:
        printf("MSG_MAKE_RESPONCE: %s\n",
            okResponce ? "ok" : "fail");
        break;
    case MSG_BUY_RESPONCE:
        printf("MSG_BUY_RESPONCE: %s\n",
            okResponce ? "ok" : "fail");
        break;
    case MSG_SELL_RESPONCE:
        printf("MSG_SELL_RESPONCE: %s\n",
            okResponce ? "ok" : "fail");
        break;
    case MSG_TURN_RESPONCE:
        printf("MSG_TURN_RESPONCE: %s\n",
            okResponce ? "ok" : "fail");
        break;
    case MSG_JOIN_RESPONCE:
        printf("MSG_JOIN_RESPONCE: %s\n",
            okResponce ? "ok" : "fail");
        break;
    case MSG_UNKNOWN:
        printf("MSG_UNKNOWN\n");
        break;
    case MSG_LEXER_ERROR:
        printf("MSG_LEXER_ERROR\n");
    }
}


const char* ServerMsgLexer::typeOfServerMsgStr[] = {
    "[Status]",
    "[Nick]",
    "[Build]",
    "[Make]",
    "[Buy]",
    "[Sell]",
    "[Turn]",
    "[Join]",
    "",
    ""
};

void ServerMsgLexer::die(int line)
{
#ifndef DAEMON
    fprintf(stderr,
        "Sorry, I'm bad lexer. See %s:%d\n", __FILE__, line);
#endif
    exit(INTERNAL_ERROR);
}

TypeOfServerMsg ServerMsgLexer::getMsgType(const String& str) const
{
    for (unsigned int i = 0;
        i < sizeof(typeOfServerMsgStr) / sizeof (char*); ++i)
    {
        if (str.isEqual(typeOfServerMsgStr[i]))
            return (TypeOfServerMsg) i;
    }

    return MSG_UNKNOWN;
}

ServerMsg* ServerMsgLexer::stStart()
{
    if (c == '\n') {
        /* Skip empty lines */
        requestNextChar = 1;
    } else if (c == '[') {
        state = ST_HEAD_START;
    } else if (c == '<') {
        timestampSkipped = 0;
        state = ST_SKIP_TO_NEXT_HEAD;
    } else {
        timestampSkipped = 0;
        state = ST_SKIP_TO_NEXT_HEAD;
    }

    return 0;
}

ServerMsg* ServerMsgLexer::stHeadStart()
{
    if (c == '\n') {
        msgType = getMsgType(tmpBuffer);
        tmpBuffer.clear();
        switch (msgType) {
        case MSG_STATUS_RESPONCE:
            timestampSkipped = 0;
            state = ST_STATUS_RESPONCE;
            requestNextChar = 1;
            break;
        case MSG_NICK_RESPONCE:
        case MSG_BUILD_RESPONCE:
        case MSG_MAKE_RESPONCE:
        case MSG_BUY_RESPONCE:
        case MSG_SELL_RESPONCE:
        case MSG_TURN_RESPONCE:
        case MSG_JOIN_RESPONCE:
            timestampSkipped = 0;
            state = ST_OK_FAIL_RESPONCE;
            requestNextChar = 1;
            break;
        case MSG_UNKNOWN:
            timestampSkipped = 0;
            state = ST_SKIP_TO_NEXT_HEAD;
            break;
        case MSG_LEXER_ERROR:
            /* Not possible. */
            die(__LINE__);
            break;
        }
    } else if (timestampSkipped && isdigit(c)) {
        timestampSkipped = 0;
        state = ST_ERROR;
    } else if (!timestampSkipped && isdigit(c)) {
        tmpBuffer.clear();
        state = ST_HEAD_SKIP_TIMESTAMP;
        requestNextChar = 1;
    } else {
        tmpBuffer += c;
        requestNextChar = 1;
    }

    return 0;
}

ServerMsg* ServerMsgLexer::stHeadSkipTimestamp()
{
    if (isdigit(c) || (c == ':') || (c == ']') || (c == ' ')) {
        /* Skip */
        requestNextChar = 1;
    } else {
        timestampSkipped = 1;
        state = ST_START;
    }

    return 0;
}

ServerMsg* ServerMsgLexer::stSkipToNextHead()
{
    if (c == '[' && tmpBuffer.isEqual("\n")) {
        tmpBuffer.clear();
        state = ST_START;
        return 0;
    }
    
    tmpBuffer.clear();
    tmpBuffer += c;
    requestNextChar = 1;

    return 0;
}

ServerMsg* ServerMsgLexer::stOkFailResponce()
{
    /* "OK" response, for commands with request semantic. */
    /* TODO: static? */
    const char msg_request_flushed[] = "Okay! "
        "Your request flushed.\n";
    const char msg_request_stored[] = "Okay! "
        "Your request stored to after-step processing.\n";
    const char msg_request_replaced[] = "Okay! "
        "Your old request replaced with this.\n";

    tmpBuffer += c;
    if (c != '\n') {
        requestNextChar = 1;
        return 0;
    }

    state = ST_START;
    ServerMsg *msg = new ServerMsg;
    msg->type = msgType;
    /* msg->okResponce is undefined. */
    msg->status = 0;

    switch (msgType) {
    case MSG_STATUS_RESPONCE:
        /* Not possible. */
        die(__LINE__);
    case MSG_NICK_RESPONCE:
        msg->okResponce = tmpBuffer.startsWith("Your username: ");
        break;
    case MSG_BUILD_RESPONCE:
    case MSG_MAKE_RESPONCE:
    case MSG_BUY_RESPONCE:
    case MSG_SELL_RESPONCE:
        msg->okResponce = tmpBuffer.isEqual(msg_request_flushed)
            || tmpBuffer.isEqual(msg_request_stored)
            || tmpBuffer.isEqual(msg_request_replaced);
        break;
    case MSG_TURN_RESPONCE:
        msg->okResponce =
            tmpBuffer.isEqual("This month completed.\n");
        break;
    case MSG_JOIN_RESPONCE:
        msg->okResponce = tmpBuffer.isEqual("Okay! "
            "Your request to participating in next game round\n");
        break;
    case MSG_UNKNOWN:
    case MSG_LEXER_ERROR:
        /* Not possible. */
        die(__LINE__);
    }

    tmpBuffer.clear();
    requestNextChar = 1;
    state = ST_START;
    return msg;
}

ServerMsg* ServerMsgLexer::stStatusResponce()
{
    tmpBuffer += c;

    if (tmpBuffer.isEqual("You are not player.\n") ||
        tmpBuffer.isEqual("Client with same username not found,"
            "try \"status --players\".\n"))
    {
        tmpBuffer.clear();
        ServerMsg *msg = new ServerMsg;
        msg->type = MSG_STATUS_RESPONCE;
        msg->okResponce = 0;
        /* msg->status is undefined */
        tmpValue = -1;
        tmpStatus = 0;
        requestNextChar = 1;
        state = ST_START;
        return msg;
    } else if (tmpBuffer.isEqual("--- Building factories ---\n")) {
        tmpBuffer.clear();
        ServerMsg *msg = new ServerMsg;
        msg->type = MSG_STATUS_RESPONCE;
        msg->okResponce = 1;
        msg->status = tmpStatus;
        tmpValue = -1;
        tmpStatus = 0;
        requestNextChar = 1;
        state = ST_START;
        return msg;
    } else if (c == ':') {
        tmpBuffer.clear();
        tmpValue = -1;
        if (tmpStatus == 0)
            tmpStatus = new MsgStatus();
        state = ST_STATUS_READ_VALUE;
    } else if (c == '\n') {
        tmpBuffer.clear();
    }

    requestNextChar = 1;
    return 0;
}

ServerMsg* ServerMsgLexer::stStatusReadValue()
{
    if (isdigit(c)) {
        tmpValue = tmpValue * 10 + (c - '0');
        requestNextChar = 1;
    } else if (tmpValue == -1) {
        requestNextChar = 1;
    } else {
        state = ST_STATUS_SKIP_TO_NEWLINE;
        tmpStatus->add(tmpValue);
    }

    return 0;
}


ServerMsg* ServerMsgLexer::stStatusSkipToNewline()
{
    if (c == '\n') {
        state = ST_STATUS_RESPONCE;
    }

    requestNextChar = 1;
    return 0;
}

ServerMsg* ServerMsgLexer::stError()
{
    ServerMsg *msg = new ServerMsg;
    msg->type = MSG_LEXER_ERROR;
    /* okResponce and status is undefined */
    state = ST_START;
    return msg;
}

ServerMsgLexer::ServerMsgLexer()
        : state(ST_START),
        timestampSkipped(0),
        tmpBuffer(),
        /* msgType is undefined */
        requestNextChar(1),
        queue(),
        /* c is undefined */
        tmpValue(-1),
        tmpStatus(0)
            {}

void ServerMsgLexer::putNewData(char *buffer, int size)
{
    queue.putNewData(buffer, size);
}

/* If new data necessary, returned 0.
 * Otherwise, returned message structure. */
ServerMsg *ServerMsgLexer::getMsg()
{
    ServerMsg *msg = 0;

    do {
        if (requestNextChar) {
            if (queue.hasNextChar()) {
                c = queue.getNextChar();
                requestNextChar = 0;
            } else {
                /* Request for new data */
                return 0;
            }
        }

        switch (state) {
        case ST_START:
            msg = stStart();
            break;
        case ST_HEAD_START:
            msg = stHeadStart();
            break;
        case ST_HEAD_SKIP_TIMESTAMP:
            msg = stHeadSkipTimestamp();
            break;
        case ST_SKIP_TO_NEXT_HEAD:
            msg = stSkipToNextHead();
            break;
        case ST_OK_FAIL_RESPONCE:
            msg = stOkFailResponce();
            break;
        case ST_STATUS_RESPONCE:
            msg = stStatusResponce();
            break;
        case ST_STATUS_READ_VALUE:
            msg = stStatusReadValue();
            break;
        case ST_STATUS_SKIP_TO_NEWLINE:
            msg = stStatusSkipToNewline();
            break;
        case ST_ERROR:
            msg = stError();
            break;
        } /* switch */
    } while (msg == 0);

    return msg;
}
