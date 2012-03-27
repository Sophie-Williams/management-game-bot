HEADERS = IndexOutOfBoundException.hpp String.hpp
SRCMODULES = String.cpp string-test.cpp
PRECOMP = $(HEADERS:.hpp=.hpp.gch)
OBJMODULES = $(SRCMODULES:.cpp=.o)
EXEC_FILE = string-test

DEFINE =
CFLAGS = -g -Wall -Wextra -ansi -pedantic $(DEFINE)

default: $(EXEC_FILE)

%.hpp.gch: %.hpp
	$(CXX) -x c++-header $(CFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

$(EXEC_FILE): $(PRECOMP) $(OBJMODULES)
	$(CXX) $(CFLAGS) $(HEADERS) $(OBJMODULES) -o $@

clean:
	rm -f *.hpp.gch *.o $(EXEC_FILE) *.core core
