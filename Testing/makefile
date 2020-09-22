CC = g++ --std=c++11
BOARD = -x c++ $(board)

simulate: main.cpp ./libraries/**/*.cpp
ifeq ($(board),)
	$(error Specify board file | usage: make board=<boardfile>)
else
	$(CC) $(LIBRARY) $^ $(BOARD) -o $@
endif

libraries/.depend: 
	echo LIBRARY = \\ > libraries/.depend
	ls -d libraries/* | xargs -I % echo -I% \\ >> libraries/.depend

include libraries/.depend