#ifndef GAME_ACTIONS_HPP_SENTRY
#define GAME_ACTIONS_HPP_SENTRY

#include "ServerMsgLexer.hpp"
#include "Socket.hpp"
#include "GameActionsException.hpp"

#define GAME_ACTIONS_BUFFER_SIZE 4096
#define MSG(str) (str), (sizeof(str) - 1)

class GameActions {
    char buffer[GAME_ACTIONS_BUFFER_SIZE];
    ServerMsgLexer lexer;
    ServerMsg* currentMsg;
    Socket socket;
    const char* winners;

    void getNextMsg();

    void expectMsg(TypeOfServerMsg type,
        bool skipFailed);

    void parseAvailable();

public:
    GameActions(const char* host,
        const char* port);

    ~GameActions();

    void join();

    void buy(int count, int cost);
    void sell(int count, int cost);
    void make(int count);
    void build(int count);
    void turn();

    bool isWin();
    const char* getWinners();

    const char* myNick();
    int getPlayers();

    int getMarketRaws();
    int getRawPrice();
    int getMarketProductions();
    int getProductionPrice();

    int getMoney();
    int getRaws();
    int getProductions();
    int getFactories();
};

#endif /* GAME_ACTIONS_HPP_SENTRY */
