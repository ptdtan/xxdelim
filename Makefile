CXX=clang++
CXXFLAGS += -g -Wall -msse4.2 -std=c++11 
all:
	$(CXX) ${CXXFLAGS} -c unittest.cpp
	$(CXX) ${CXXFLAGS} -c xxdelim.cpp
	$(CXX) ${CXXFLAGS} -c block_info/genblock.cpp
	$(CXX) ${CXXFLAGS} -o unittest unittest.o genblock.o 
	$(CXX) ${CXXFLAGS} -o xxdelim xxdelim.o genblock.o 
