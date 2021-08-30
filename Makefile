all:
	g++ -msse4.2 -std=c++11 -c main.cpp
	g++ -msse4.2 -std=c++11 -c block_info/genblock.cpp
	g++ -o xxdelim main.o genblock.o -msse4.2 -std=c++11
