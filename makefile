TLIBS = -pthread
LIBS = -lcryptopp
FLAGS = -O3 -Wextra -Wall
CPP = g++ -std=c++17
INCREDIBLESHITLANG = -lstdc++fs

all: mktable lookup

test: test.cpp
	$(CPP) $(TLIBS) $(LIBS) -o $@ $^
	./test

utils.o: utils.hpp
	$(CPP) $(FLAGS) -c  utils.cpp

types.o: types.cpp types.hpp utils.o config.h
	$(CPP) $(FLAGS) -c types.cpp

rainbow-table.o: rainbow-table.cpp utils.o types.o
	$(CPP) $(TLIBS) $(FLAGS) -c rainbow-table.cpp

rainbow-lookup.o: rainbow-lookup.cpp types.o utils.o
	$(CPP) $(FLAGS) -c rainbow-lookup.cpp

mktable: rainbow-table.o types.o utils.o
	$(CPP) $(TLIBS) $(LIBS) $(FLAGS) -o $@ $^ $(INCREDIBLESHITLANG)

lookup: rainbow-lookup.o types.o utils.o
	$(CPP) $(FLAGS) -std=c++17 -o $@ $^

clean:
	rm -v -f *.o
	rm -v -f mktable
	rm -v -f lookup
