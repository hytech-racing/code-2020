CC = g++ --std=c++11
BOARD = -x c++ $(board)

simulate: main.cpp ./Libraries/**/*.cpp
ifeq ($(board),)
	$(error Specify board file | usage: make board=<boardfile>)
else
	$(CC) $(LIBRARY) $^ $(BOARD) -o $@
endif

Libraries/.depend: 
	echo LIBRARY = \\ > Libraries/.depend
	ls -d Libraries/* | xargs -I % echo -I% \\ >> Libraries/.depend

include Libraries/.depend