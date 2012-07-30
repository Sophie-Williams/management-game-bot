#include "GameActions.hpp"

void GameActions::getNextMsg()
{
    if (currentMsg)
        delete currentMsg;

    currentMsg = lexer.getMsg();

    while (currentMsg == 0) {
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

        currentMsg = lexer.getMsg();
    }

#if !defined(DAEMON) && defined(GAME_ACTIONS_DEBUG)
    currentMsg->print(stderr);
#endif
}

void GameActions::expectMsg(TypeOfServerMsg type, bool skipFailed)
{
    do {
        getNextMsg();

        if (currentMsg->type == MSG_WINNERS_ASYNC) {
            int size = strlen(currentMsg->str) + 1;
            winners = new char[size];
            memcpy(winners, currentMsg->str, size);
            return;
        }
    } while ((skipFailed && !currentMsg->ok) ||
        (currentMsg->type != type));

    if (currentMsg->ok)
        return;

    throw GameActionsException("Fail message with expected type.",
        __FILE__, __LINE__);
}

void GameActions::parseAvailable()
{
    do {
        if (currentMsg)
            delete currentMsg;

        currentMsg = lexer.getMsg();

        while (currentMsg != 0) {
            if (currentMsg->type == MSG_WINNERS_ASYNC) {
                int size = strlen(currentMsg->str) + 1;
                winners = new char[size];
                memcpy(winners, currentMsg->str, size);
                return;
            }

            if (currentMsg)
                delete currentMsg;

            currentMsg = lexer.getMsg();
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
    currentMsg(0),
    socket(host, port),
    winners(0)
{}

GameActions::~GameActions()
{
    if (socket.isConnected()) {
        try {
            socket.disconnect();
        } catch(SocketIOException& ex) {
#ifndef DAEMON
            printf("%s", ex.toString());
#endif
        }
    }

    if (currentMsg)
        delete currentMsg;
    if (winners)
        delete[] winners;
}

void GameActions::join()
{
    socket.connect();
    socket.write(MSG("join\r\n"));
    expectMsg(MSG_JOIN_RESPONCE, false);
    expectMsg(MSG_ROUNDS_ASYNC, true);
}

void GameActions::buy(int count, int cost)
{
    char *str;
    asprintf(&str, "buy %d %d\r\n", count, cost);
    socket.write(str, strlen(str));
    free(str);

    expectMsg(MSG_BUY_RESPONCE, false);
}

void GameActions::sell(int count, int cost)
{
    char *str;
    asprintf(&str, "sell %d %d\r\n", count, cost);
    socket.write(str, strlen(str));
    free(str);

    expectMsg(MSG_SELL_RESPONCE, false);
}

void GameActions::make(int count)
{
    char *str;
    asprintf(&str, "make %d\r\n", count);
    socket.write(str, strlen(str));
    free(str);

    expectMsg(MSG_MAKE_RESPONCE, false);
}

void GameActions::build(int count)
{
    char *str;
    asprintf(&str, "build %d\r\n", count);
    socket.write(str, strlen(str));
    free(str);

    expectMsg(MSG_BUILD_RESPONCE, false);
}

void GameActions::turn()
{
    socket.write(MSG("turn\r\n"));
    expectMsg(MSG_TURN_RESPONCE, true);
    expectMsg(MSG_MONTH_COMPLETED, true);
}

bool GameActions::isWin()
{
    parseAvailable();
    return (winners != 0);
}

const char* GameActions::getWinners()
{
    char* newWinners;

    parseAvailable();

    if (winners) {
        int size = strlen(winners) + 1;
        newWinners = new char[size];
        memcpy(newWinners, winners, size);
    } else {
        newWinners = new char[1];
        newWinners[0] = '\0';
    }

    return newWinners;
}

const char* GameActions::myNick()
{
    socket.write(MSG("nick\r\n"));
    expectMsg(MSG_NICK_RESPONCE, false);
    int size = strlen(currentMsg->str) + 1;
    char* nick = new char[size];
    memcpy(nick, currentMsg->str, size);
    return nick;
}

int GameActions::getPlayers()
{
    socket.write(MSG("status --server\r\n"));
    expectMsg(MSG_STATUS_RESPONCE, false);
    int players = currentMsg->status->getPlayers();
    return players;
}

int GameActions::getMarketRaws()
{
    socket.write(MSG("status --market\r\n"));
    expectMsg(MSG_STATUS_RESPONCE, false);
    int marketRaws = currentMsg->status->getMarketRaws();
    return marketRaws;
}

int GameActions::getRawPrice()
{
    socket.write(MSG("status --market\r\n"));
    expectMsg(MSG_STATUS_RESPONCE, false);
    int rawPrice = currentMsg->status->getMarketRawMinPrice();
    return rawPrice;
}

int GameActions::getMarketProductions()
{
    socket.write(MSG("status --market\r\n"));
    expectMsg(MSG_STATUS_RESPONCE, false);
    int marketProductions = currentMsg->status->getMarketProductions();
    return marketProductions;
}

int GameActions::getProductionPrice()
{
    socket.write(MSG("status --market\r\n"));
    expectMsg(MSG_STATUS_RESPONCE, false);
    int productionPrice = currentMsg->status->getMarketProdMaxPrice();
    return productionPrice;
}

int GameActions::getMoney()
{
    socket.write(MSG("status\r\n"));
    expectMsg(MSG_STATUS_RESPONCE, false);
    int money = currentMsg->status->getPlayerMoney();
    return money;
}

int GameActions::getRaws()
{
    socket.write(MSG("status\r\n"));
    expectMsg(MSG_STATUS_RESPONCE, false);
    int raws = currentMsg->status->getPlayerRaws();
    return raws;
}

int GameActions::getProductions()
{
    socket.write(MSG("status\r\n"));
    expectMsg(MSG_STATUS_RESPONCE, false);
    int productions = currentMsg->status->getPlayerProductions();
    return productions;
}

int GameActions::getFactories()
{
    socket.write(MSG("status\r\n"));
    expectMsg(MSG_STATUS_RESPONCE, false);
    int factories = currentMsg->status->getPlayerFactories();
    return factories;
}
