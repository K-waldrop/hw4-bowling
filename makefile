CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic

.PHONY: all clean

all: a.out

a.out: main.o bowling_frame.o
	$(CXX) $(CXXFLAGS) $^ -o $@

main.o bowling_frame.o: bowling_frame.h

clean:
	rm -f main.o bowling_frame.o a.out
