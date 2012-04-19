#include <stdio.h>
#include "Socket.hpp"
#include "ServerMsgLexer.hpp"
#include "Exception.hpp"

#define MSG(str) (str), (sizeof(str) - 1)
#define MIN(n1, n2) (((n1) <= (n2)) ? (n1) : (n2))

#define BUFFER_SIZE 4096

class BaldBotException : public Exception {
    const char* description;
    const char* winners;

public:
    BaldBotException(const char* aDescription,
        const char* aWinners,
        const char* aFile, int aLine)
        : Exception(aFile, aLine),
        description(aDescription),
        winners(aWinners)
    {}

    BaldBotException(const char* aDescription,
        const char* aFile, int aLine)
        : Exception(aFile, aLine),
        description(aDescription),
        winners(0)
    {}

    const char* getDescription() const
    {
        return description;
    }

    bool isWinnersException() const
    {
        return (winners != 0);
    }

    const char* getWinners() const
    {
        return winners;
    }

    const char* toString() const
    {
        String msg("BaldBotExpection(\"");

        msg += description;
        msg += "\"); ";
        msg += getFile();

        char* lineStr = 0;
        asprintf(&lineStr, ":%d", getLine());

        msg += lineStr;

        if (winners != 0) {
            msg += "; ";
            msg += winners;
        }

        msg += "\n";

        return msg.getCharPtr();
    }
};

struct BotState {
   char buffer[BUFFER_SIZE];
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
                BUFFER_SIZE * sizeof(char), &(state.readed));
            if (notEOF) {
                state.lexer.putNewData(state.buffer, state.readed);
            } else {
//                state.lexer.putEOF();
                throw BaldBotException("Got EOF.",
                    __FILE__, __LINE__);
            }

            msg = state.lexer.getMsg();
        }

//        msg->print(stdout);
        if (msg->type == MSG_WINNERS_ASYNC) {
            throw BaldBotException("Winners", msg->str,
                __FILE__, __LINE__);
        }
    } while ((skipFailed && !msg->ok) || (msg->type != type));

    if (msg->ok)
        return msg;

    throw BaldBotException("Fail message with expected type.",
        __FILE__, __LINE__);
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

        state.socket.write(MSG("nick\r\n"));
        msg = expectMsg(state, MSG_NICK_RESPONCE, false);
        printf("%s", msg->str);

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
    } catch(BaldBotException& ex) {
        if (ex.isWinnersException()) {
            printf("%s", ex.getWinners());
            return 0;
        } else {
            printf("%s", ex.toString());
            return 1;
        }
    } catch(Exception& ex) {
        printf("%s", ex.toString());
        return 1;
    }

    return 0;
}
