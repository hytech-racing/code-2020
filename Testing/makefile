CC = g++ --std=c++11
BOARD = -x c++ $(board)

depend:
	bash scripts/depend

links:
	bash scripts/symlink

setup: depend links
	git config core.excludesfiles Testing/Libraries/.symlinks

include Libraries/.depend

linker/main.o: main.cpp
	$(CC) -c $(LIBS) -o $@ $<

linker/%.o: Libraries/%.cpp
	install -D /dev/null $@
	rm $@
	$(CC) -c $(LIBS) -o $@ $<

simulate: linker/main.o $(DEPS)
ifeq ($(board),)
	$(error Specify board file | usage: make board=<boardfile>)
else
	$(CC) -Wall -g $(LIBS) $^ $(BOARD) -o $@
endif