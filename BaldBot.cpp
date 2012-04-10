#include <stdio.h>
#include "Socket.hpp"
#include "ServerMsgLexer.hpp"

#define MSG(str) (str), (sizeof(str) - 1)
#define MIN(n1, n2) (((n1) <= (n2)) ? (n1) : (n2))

static const int bufferSize = 4096;

struct BotState {
   char buffer[bufferSize];
   ServerMsgLexer lexer;
   Socket socket;
   int readed;

   BotState(const char* host, const char* port)
       : socket(host, port), readed(0) {}
};

ServerMsg* expectMsg(BotState& state, TypeOfServerMsg type, bool skipFailed)
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
                throw 2;
            }

            msg = state.lexer.getMsg();
        }

//        msg->print(stdout);
    } while ((skipFailed && !msg->ok) || (msg->type != type));

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
        send("make"); // min raws and factories
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

        do {
            state.socket.write(MSG("status --market\r\n"));
            msg = expectMsg(state, MSG_STATUS_RESPONCE, false);

            int marketRaws = msg->status->getMarketRaws();
            int marketRawMinPrice = msg->status->getMarketRawMinPrice();
            int marketProductions = msg->status->getMarketProductions();
            int marketProdMaxPrice = msg->status->getMarketProdMaxPrice();

            state.socket.write(MSG("status\r\n"));
            msg = expectMsg(state, MSG_STATUS_RESPONCE, false);

            int playerMoney = msg->status->getPlayerMoney();
            int playerRaws = msg->status->getPlayerRaws();
            int playerProductions = msg->status->getPlayerProductions();
            int playerFactories = msg->status->getPlayerFactories();

            printf("Money: %d\n", playerMoney);

            int count = MIN(marketRaws, playerFactories);
            int cost = marketRawMinPrice;

            char *str;
            asprintf(&str, "buy %d %d\r\n", count, cost);
            state.socket.write(str, strlen(str));
            free(str);

            msg = expectMsg(state, MSG_BUY_RESPONCE, false);

            count = MIN(marketProductions, playerProductions);
            cost = marketProdMaxPrice;

            asprintf(&str, "sell %d %d\r\n", count, cost);
            state.socket.write(str, strlen(str));
            free(str);

            msg = expectMsg(state, MSG_SELL_RESPONCE, false);

            count = MIN(playerRaws, playerFactories);

            asprintf(&str, "make %d\r\n", count);
            state.socket.write(str, strlen(str));
            free(str);

            msg = expectMsg(state, MSG_MAKE_RESPONCE, false);

            state.socket.write(MSG("turn\r\n"));
            /* TODO: only responce, not async;
             * true in 3 argument in temporally. */
            msg = expectMsg(state, MSG_TURN_RESPONCE, true);

            msg = expectMsg(state, MSG_MONTH_COMPLETED, false);
        } while (1);

        state.socket.disconnect();
//        lexer.putEOF();
    } catch(Exception& ex) {
        printf("%s", ex.toString());
    } catch(int ex) {
        printf("Exception: %d\n", ex);
    }

    return 0;
}
