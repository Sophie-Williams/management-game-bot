#include "ScriptLexer.hpp"

void ScriptLexer::die(int line)
{
#ifndef DAEMON
    fprintf(stderr,
        "Sorry, I'm bad lexer. See %s:%d\n", __FILE__, line);
#endif
    exit(INTERNAL_ERROR);
}
