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
    Socket socket;
    const char* winners;

    ServerMsg* expectMsg(TypeOfServerMsg type,
        bool skipFailed);

    void expectOnlyOk(TypeOfServerMsg type);
    void expectFirstOk(TypeOfServerMsg type);

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
};

#endif /* GAME_ACTIONS_HPP_SENTRY */
