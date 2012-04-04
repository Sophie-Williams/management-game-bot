SRCMODULES = CharQueue.cpp ScriptLexer.cpp ServerMsgLexer.cpp String.cpp Socket.cpp
TEST_SRC_FILES = ScriptLexerTest.cpp ServerMsgLexerTest.cpp StringTest.cpp SocketTest.cpp
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

ifneq (clean, $(MAKECMDGOALS))
ifneq (clang_analyze_clean, $(MAKECMDGOALS))
-include deps.mk
endif
endif

deps.mk: $(SRCMODULES) $(HEADERS)
	$(CXX) -MM $^ > $@

clean:
	rm -f *.o $(TEST_EXEC_FILES) deps.mk *.core core

clang_analyze_clean:
	rm -f *.hpp.gch *.plist
