#include <unistd.h>
#include <stdio.h>
#include "ServerMsgLexer.hpp"
#include "Exception.hpp"

#define READ_ERROR(readValue) ((readValue) == -1)
#define READ_EOF(readValue) ((readValue) == 0)

void printMessages(ServerMsgLexer& lexer)
{
    ServerMsg *msg;

    do {
        msg = lexer.getMsg();

        if (msg == 0)
            return;

        msg->print(stdout);

        if (msg->type == MSG_LEXER_ERROR)
            exit(INTERNAL_ERROR);
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
    } catch(Exception& ex) {
        printf("Exception: %s\n", ex.toString());
    }

    return 0;
}
