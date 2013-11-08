.PHONY : clean
CC=g++
CFLAGS=-std=c++11 -Wall -g

life : simulator.o miner_parser.tab.o lex.yy.o miner_type.o life.o
	$(CC) $(CFLAGS) -o $@ $^

life.o : life.cc miner_parser.h
	$(CC) $(CFLAGS) -c life.cc

simulator.o : simulator.cc simulator.h
	$(CC) $(CFLAGS) -c simulator.cc

miner_parser.tab.o : miner_parser.tab.c
	$(CC) $(CFLAGS) -c $^

miner_type.o : miner_type.cc miner_type.h
	$(CC) $(CFLAGS) -c miner_type.cc

lex.yy.o : lex.yy.c
	$(CC) $(CFLAGS) -c $^

miner_parser.tab.c : miner_parser.y miner_type.h utils.h
	bison -d miner_parser.y

lex.yy.c : miner_type.h miner_parser.tab.c miner_parser.l
	flex --nounput miner_parser.l

clean :
	rm -f miner_parser.tab.c miner_parser.tab.h lex.yy.c *.o life
