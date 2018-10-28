LIBS = -pthread -lcryptopp
FLAGS = -O3 -Wpedantic -Wall
CPP = clang++ -std=c++17
INCREDIBLESHITLANG = -lstdc++fs

all: mktable

test: test.cpp
	$(CPP) $(FLAGS) $(LIBS) -o test test.cpp
	./test

utils.o: utils.hpp
	$(CPP) $(FLAGS) -c utils.cpp

types.o: types.cpp types.hpp utils.o config.h
	$(CPP) $(FLAGS) -c types.cpp

rainbow-table.o: rainbow-table.cpp utils.o types.o
	$(CPP) $(LIBS) $(FLAGS) -c rainbow-table.cpp

mktable: rainbow-table.o types.o utils.o
	$(CPP) $(LIBS) $(FLAGS) -o $@ $^ $(INCREDIBLESHITLANG)

run: mktable
	./mktable

clean:
	rm -v -f *.o
	rm -v -f table
