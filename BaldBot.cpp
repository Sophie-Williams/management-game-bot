#include <stdio.h>
#include "Socket.hpp"
#include "ServerMsgLexer.hpp"

#define MSG(str) (str), sizeof(str)

static const int bufferSize = 4096;

struct BotState {
   char buffer[bufferSize];
   ServerMsgLexer lexer;
   Socket socket;
   int readed;

   BotState(const char* host, const char* port)
       : socket(host, port), readed(0) {}
};

ServerMsg* expectMsg(BotState& state, TypeOfServerMsg type, bool skipIfNotOk)
{
    ServerMsg *msg = 0;

    do {
        msg = state.lexer.getMsg();

        while (msg == 0) {
            bool notEOF = state.socket.read(state.buffer,
                bufferSize * sizeof(char), &(state.readed));
            if (notEOF) {
                state.lexer.putNewData(state.buffer, state.readed);
            } else {
//                state.lexer.putEOF();
                printf("Got EOF, exiting...");
                exit(0);
            }

            msg = state.lexer.getMsg();
        }

        msg->print(stdout);
    } while ((skipIfNotOk && !msg->ok) || (msg->type != type));

    if (msg->ok)
        return msg;

    throw 1; /* TODO */
}

int main(int argc, char** argv)
{
    if (argc != 3) {
        fprintf(stderr, "Usage: ./BaldBot host port\n");
        return 1;
    }

#if 0
    send("join");
    get("join-ok");
    while (!endround) {
        send("buy"); // min market and factories
        get("buy-ok");
        send("sell"); // min market and productions
        get("sell-ok");
        send("make"); // min raws and productions
        get("make-ok");
        send("turn");
        get("turn-ok");
        get("newround");
    }
#endif

    try {
        BotState state(argv[1], argv[2]);
        ServerMsg *msg;
        state.socket.connect();
        state.socket.write(MSG("join\r\n"));
        msg = expectMsg(state, MSG_JOIN_RESPONCE, false);
        msg = expectMsg(state, MSG_ROUNDS_ASYNC, true);
        state.socket.disconnect();
//        lexer.putEOF();
    } catch(Exception& ex) {
        printf("%s", ex.toString());
    } catch(int ex) {
        printf("Exception: %d\n", ex);
    }

    return 0;
}
