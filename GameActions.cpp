#include "GameActions.hpp"

ServerMsg* GameActions::expectMsg(TypeOfServerMsg type, bool skipFailed)
{
    ServerMsg *msg = 0;

    do {
        msg = lexer.getMsg();

        while (msg == 0) {
            int readed;
            bool notEOF = socket.read(buffer,
                GAME_ACTIONS_BUFFER_SIZE
                * sizeof(char), &readed);
            if (notEOF) {
                lexer.putNewData(buffer, readed);
            } else {
                throw GameActionsException("Got EOF"
                    ", expect ok response.",
                    __FILE__, __LINE__);
            }

            msg = lexer.getMsg();
        }

        if (msg->type == MSG_WINNERS_ASYNC) {
            winners = msg->str;
            return 0;
        }
    } while ((skipFailed && !msg->ok) || (msg->type != type));

    if (msg->ok)
        return msg;

    throw GameActionsException("Fail message with expected type.",
        __FILE__, __LINE__);
}

void GameActions::expectOnlyOk(TypeOfServerMsg type)
{
    expectMsg(type, false);
}

void GameActions::expectFirstOk(TypeOfServerMsg type)
{
    expectMsg(type, true);
}

void GameActions::parseAvailable()
{
    do {
        ServerMsg *msg = lexer.getMsg();

        while (msg != 0) {
            if (msg->type == MSG_WINNERS_ASYNC) {
                winners = msg->str;
                return;
            }

            msg = lexer.getMsg();
        }

        if (! socket.isReadAvailable())
            return;

        int readed;
        bool notEOF = socket.read(buffer,
            GAME_ACTIONS_BUFFER_SIZE
            * sizeof(char), &readed);
        if (notEOF) {
            lexer.putNewData(buffer, readed);
        } else {
            return;
        }
    } while (1);
}

GameActions::GameActions(const char* host, const char* port)
    : lexer(),
    socket(host, port),
    winners(0)
{}

GameActions::~GameActions()
{
    if (socket.isConnected()) {
        socket.disconnect();
    }
}

void GameActions::join()
{
    socket.connect();
    socket.write(MSG("join\r\n"));
    expectOnlyOk(MSG_JOIN_RESPONCE);
    expectFirstOk(MSG_ROUNDS_ASYNC);
}

void GameActions::buy(int count, int cost)
{
    char *str;
    asprintf(&str, "buy %d %d\r\n", count, cost);
    socket.write(str, strlen(str));
    free(str);

    expectOnlyOk(MSG_BUY_RESPONCE);
}

void GameActions::sell(int count, int cost)
{
    char *str;
    asprintf(&str, "sell %d %d\r\n", count, cost);
    socket.write(str, strlen(str));
    free(str);

    expectOnlyOk(MSG_SELL_RESPONCE);
}

void GameActions::make(int count)
{
    char *str;
    asprintf(&str, "make %d\r\n", count);
    socket.write(str, strlen(str));
    free(str);

    expectOnlyOk(MSG_MAKE_RESPONCE);
}

void GameActions::build(int count)
{
    char *str;
    asprintf(&str, "build %d\r\n", count);
    socket.write(str, strlen(str));
    free(str);

    expectOnlyOk(MSG_BUILD_RESPONCE);
}

void GameActions::turn()
{
    socket.write(MSG("turn\r\n"));

    expectOnlyOk(MSG_TURN_RESPONCE);
    expectFirstOk(MSG_MONTH_COMPLETED);
}

bool GameActions::isWin()
{
    parseAvailable();
    return (winners != 0);
}

const char* GameActions::getWinners()
{
    parseAvailable();
    return winners;
}
