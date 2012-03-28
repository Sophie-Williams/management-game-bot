#!/bin/sh

g++ -Wall -Wextra -g -ansi -pedantic -c CharQueue.cpp -o CharQueue.o
g++ -Wall -Wextra -g -ansi -pedantic -c ServerMsgLexer.cpp -o ServerMsgLexer.o
g++ -Wall -Wextra -g -ansi -pedantic String/String.o CharQueue.o ServerMsgLexer.o ServerMsgLexerTest.cpp -o ServerMsgLexerTest
g++ -Wall -Wextra -g -ansi -pedantic -c ScriptLexer.cpp -o ScriptLexer.o
