CXXFLAGS += -Wall -msse4.2 -std=c++11 
all:
	g++ ${CXXFLAGS} -c main.cpp
	g++ ${CXXFLAGS} -c block_info/genblock.cpp
	g++ ${CXXFLAGS} -o xxdelim main.o genblock.o 
