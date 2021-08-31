CXXFLAGS += -g -Wall -msse4.2 -std=c++11 
all:
	g++ ${CXXFLAGS} -c unittest.cpp
	g++ ${CXXFLAGS} -c xxdelim.cpp
	g++ ${CXXFLAGS} -c block_info/genblock.cpp
	g++ ${CXXFLAGS} -o unittest unittest.o genblock.o 
	g++ ${CXXFLAGS} -o xxdelim xxdelim.o genblock.o 
