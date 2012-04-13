SRCMODULES = CharQueue.cpp ScriptLexer.cpp ServerMsgLexer.cpp String.cpp Socket.cpp ParserTables.cpp Parser.cpp
TEST_SRC_FILES = ScriptLexerTest.cpp ServerMsgLexerTest.cpp StringTest.cpp SocketTest.cpp BaldBot.cpp ParserTest.cpp
OBJMODULES = $(SRCMODULES:.cpp=.o)
HEADERS = $(SRCMODULES:.cpp=.hpp)
TEST_EXEC_FILES = $(TEST_SRC_FILES:.cpp=)

DEFINE =
CXXFLAGS = -g -Wall -Wextra -ansi -pedantic $(DEFINE)

default: $(TEST_EXEC_FILES)

%.o: %.cpp %.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

ScriptLexerTest: CharQueue.o ScriptLexer.o String.o
	$(CXX) $(CXXFLAGS) $^ $@.cpp -o $@

ServerMsgLexerTest: CharQueue.o ServerMsgLexer.o String.o
	$(CXX) $(CXXFLAGS) $^ $@.cpp -o $@

StringTest: String.o
	$(CXX) $(CXXFLAGS) $^ $@.cpp -o $@

SocketTest: String.o Socket.o
	$(CXX) $(CXXFLAGS) $^ $@.cpp -o $@

BaldBot: CharQueue.o ServerMsgLexer.o String.o Socket.o
	$(CXX) $(CXXFLAGS) $^ $@.cpp -o $@

ParserTest: CharQueue.o ScriptLexer.o String.o ParserTables.o Parser.o
	$(CXX) $(CXXFLAGS) $^ $@.cpp -o $@

ifneq (clean, $(MAKECMDGOALS))
ifneq (clang_analyze_clean, $(MAKECMDGOALS))
ifneq (graph_clean, $(MAKECMDGOALS))
ifneq (graph, $(MAKECMDGOALS))
-include deps.mk
endif
endif
endif
endif

deps.mk: $(SRCMODULES) $(HEADERS)
	$(CXX) -MM $^ > $@

graph:
	fig2dev -L svg ScriptLexer-graph.fig ScriptLexer-graph.svg
	fig2dev -L ps ScriptLexer-graph.fig ScriptLexer-graph.ps
	ps2pdf ScriptLexer-graph.ps ScriptLexer-graph.pdf

clean:
	rm -f *.o $(TEST_EXEC_FILES) deps.mk *.core core

graph_clean:
	rm -f ScriptLexer-graph.svg ScriptLexer-graph.ps ScriptLexer-graph.pdf

clang_analyze_clean:
	rm -f *.hpp.gch *.plist
