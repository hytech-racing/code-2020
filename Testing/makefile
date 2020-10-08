CC = g++ --std=c++11
BOARD = -x c++ $(board)

.PHONY: depend
depend:
	bash depend

include Libraries/.depend

Linker/main.o: main.cpp
	install -D /dev/null $@
	rm $@
	$(CC) -c $(LIBS) -o $@ $<

Linker/sim/%.o: Libraries/%.cpp
	install -D /dev/null $@
	rm $@
	$(CC) -c $(LIBS) -o $@ $<

Linker/ext/%.o: ../Libraries/%.cpp
	install -D /dev/null $@
	rm $@
	$(CC) -c $(LIBS) -o $@ $<

.PHONY: simulate
simulate: Linker/main.o $(DEPS)
ifeq ($(board),)
	$(error Specify board file | usage: make board=<boardfile>)
else
	$(CC) -Wall -g $(LIBS) $^ $(BOARD) -o $@
endif