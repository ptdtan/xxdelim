CXXFLAGS=-Wall -msse4.2 -std=c++11
all:
	g++ ${CXXFLAGS} -c genblock.cpp
	g++ ${CXXFLAGS} -c testgenblock.cpp
	g++ ${CXXFLAGS} -o testgenblock genblock.o testgenblock.o
