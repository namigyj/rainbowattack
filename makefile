TLIBS = -pthread
LIBS = -lcryptopp $(TLIBS)
FLAGS = -O2 -Wextra -Wall -flto -fuse-ld=lld
CPP = clang++ -std=c++17
INCREDIBLESHITLANG = -lstdc++fs

.PHONY: all
all: mktable lookup

test: test.cpp
	$(CPP) $(TLIBS) $(LIBS) -o $@ $^
	./test

utils.o: utils.hpp config.h
	$(CPP) $(FLAGS) -c utils.cpp -o $@

types.o: types.cpp types.hpp utils.o config.h
	$(CPP) $(FLAGS) -c types.cpp -o $@

rainbow-table.o: rainbow-table.cpp utils.o types.o
	$(CPP) $(TLIBS) $(FLAGS) -c rainbow-table.cpp -o $@

rainbow-lookup.o: rainbow-lookup.cpp types.o utils.o
	$(CPP) $(FLAGS) -c rainbow-lookup.cpp -o $@

mktable: rainbow-table.o types.o utils.o
	$(CPP) $(FLAGS) $(LIBS) -o $@ $^ $(INCREDIBLESHITLANG)

lookup: rainbow-lookup.o types.o utils.o
	$(CPP) $(FLAGS) $(LIBS) -o $@ $^

clean:
	rm -v -f *.o
	rm -v -f mktable lookup
