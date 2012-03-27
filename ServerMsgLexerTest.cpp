#include <unistd.h>
#include <stdio.h>
#include "ServerMsgLexer.hpp"

#define READ_ERROR(readValue) ((readValue) == -1)
#define READ_EOF(readValue) ((readValue) == 0)

void printMessages(ServerMsgLexer& lexer)
{
    ServerMsg *msg;

    do {
        msg = lexer.getMsg();

        if (msg == 0)
            return;

        switch (msg->type) {
        case MSG_STATUS_RESPONCE:
            printf("MSG_STATUS_RESPONCE\n");
            break;
        case MSG_NICK_RESPONCE:
            printf("MSG_NICK_RESPONCE: %s\n", msg->okResponce ? "ok" : "fail");
            break;
        case MSG_BUILD_RESPONCE:
            printf("MSG_BUILD_RESPONCE: %s\n", msg->okResponce ? "ok" : "fail");
            break;
        case MSG_MAKE_RESPONCE:
            printf("MSG_MAKE_RESPONCE: %s\n", msg->okResponce ? "ok" : "fail");
            break;
        case MSG_BUY_RESPONCE:
            printf("MSG_BUY_RESPONCE: %s\n", msg->okResponce ? "ok" : "fail");
            break;
        case MSG_SELL_RESPONCE:
            printf("MSG_SELL_RESPONCE: %s\n", msg->okResponce ? "ok" : "fail");
            break;
        case MSG_TURN_RESPONCE:
            printf("MSG_TURN_RESPONCE: %s\n", msg->okResponce ? "ok" : "fail");
            break;
        case MSG_JOIN_RESPONCE:
            printf("MSG_JOIN_RESPONCE: %s\n", msg->okResponce ? "ok" : "fail");
            break;
        case MSG_UNKNOWN:
            printf("MSG_UNKNOWN\n");
            break;
        case MSG_LEXER_ERROR:
            printf("MSG_LEXER_ERROR\n");
            exit(INTERNAL_ERROR);
        }
    } while (1);
}

int main()
{
    ServerMsgLexer lexer;
    char buffer[4096];
    int readValue;

    try {
        do {
            readValue = read(STDIN_FILENO, &buffer, sizeof(buffer));
            if (READ_ERROR(readValue)) {
                perror("read");
                break;
            } else if (READ_EOF(readValue)) {
                printf("[EOF]\n");
                break;
            } else {
                lexer.putNewData(buffer, readValue);
                printMessages(lexer);
            }
        } while (1);
    }
    catch(int ex)
    {
        printf("Exception: %d\n", ex);
    }

    return 0;
}
