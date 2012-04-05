#include <stdio.h>
#include "Socket.hpp"
#include "ServerMsgLexer.hpp"

int main(int argc, char** argv)
{
    if (argc != 3) {
        fprintf(stderr, "Usage: ./BaldBot host port\n");
        return 1;
    }

    ServerMsgLexer lexer;
    Socket s(argv[1], argv[2]);
    char buffer[4096];
    int readed;

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
        s.connect();
        while(s.read(buffer, sizeof(buffer), &readed)) {
            lexer.putNewData(buffer, readed);
        } while (1);
        s.disconnect();
//        lexer.putEOF();
    } catch(Exception& ex) {
        printf("%s", ex.toString());
    } catch(int ex) {
        printf("Exception: %d\n", ex);
    }

    return 0;
}
