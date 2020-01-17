TLIBS = -pthread
LIBS = -lcryptopp $(TLIBS)
FLAGS = -O2 -Wextra -Wall -flto -fuse-ld=lld
CPP = clang++ -std=c++17
INCREDIBLESHITLANG = -lstdc++fs

build_dir = ./build

.PHONY: all
all: mktable lookup

test: test.cpp
	$(CPP) $(TLIBS) $(LIBS) -o $@ $^
	./test

utils.o: utils.hpp config.h
	$(CPP) $(FLAGS) -c utils.cpp -o $(build_dir)/$@

types.o: types.cpp types.hpp utils.o config.h
	$(CPP) $(FLAGS) -c types.cpp -o $(build_dir)/$@

rainbow-table.o: rainbow-table.cpp utils.o types.o
	$(CPP) $(TLIBS) $(FLAGS) -c rainbow-table.cpp -o $(build_dir)/$@

rainbow-lookup.o: rainbow-lookup.cpp types.o utils.o
	$(CPP) $(FLAGS) -c rainbow-lookup.cpp -o $(build_dir)/$@

mktable: rainbow-table.o types.o utils.o
	$(CPP) $(FLAGS) $(LIBS) -o $@ $(addprefix $(build_dir)/, $^) $(INCREDIBLESHITLANG)

lookup: rainbow-lookup.o types.o utils.o
	$(CPP) $(FLAGS) $(TLIBS) -std=c++17 -o $@ $(addprefix $(build_dir)/, $^)

clean:
	rm -v -f $(build_dir)/*.o
	rm -v -rf $(build_dir)
	rm -v -f *.o
	rm -v -f mktable
	rm -v -f lookup
