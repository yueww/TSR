testStorage : produceFlows.o lsh.o flowResort.o index.o myToolFunc.o compressIndex.o \
	exportDataAndIndex.o processFlows.o test.o
	g++ -o testStorage -g produceFlows.o lsh.o flowResort.o index.o myToolFunc.o\
		compressIndex.o exportDataAndIndex.o processFlows.o test.o -lpthread
produceFlows.o : produceFlows.c produceFlows.h common.h 
	g++ -c -g produceFlows.c
processFlows.o : processFlows.c processFlows.h common.h trieIndex.h
	g++ -std=c++11 -c -g processFlows.c
lsh.o : lsh.cpp lsh.h common.h
	g++ -std=c++11 -c -g lsh.cpp
flowResort.o : flowResort.cpp flowResort.h lsh.h common.h
	g++ -std=c++11 -c -g flowResort.cpp
index.o : index.cpp index.h trieIndex.h common.h
	g++ -c -g index.cpp
myToolFunc.o : myToolFunc.c myToolFunc.h
	g++ -c -g myToolFunc.c
compressIndex.o : compressIndex.cpp compressIndex.h common.h trieIndex.h
	g++ -c -g compressIndex.cpp
exportDataAndIndex.o : exportDataAndIndex.cpp exportDataAndIndex.h common.h trieIndex.h myToolFunc.h
	g++ -c -g exportDataAndIndex.cpp
test.o : test.c test.h common.h trieIndex.h lsh.h
	g++ -std=c++11 -c -g test.c
clean :
	rm *.o testStorage 
