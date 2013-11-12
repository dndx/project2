.PHONY : clean all
CC=g++
CFLAGS=-std=c++11 -Wall -DNDEBUG #-g
QT_INCLUDE=-I/usr/lib64/qt4/mkspecs/linux-g++ -I. -I/usr/include/QtCore -I/usr/include/QtGui
QT_LIBS=-L/usr/lib64 -lQtGui -lQtCore -lpthread -DQT_NO_DEBUG -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED

all: life life_gui

life_gui : simulator.o miner_parser.tab.o lex.yy.o miner_type.o LifeGrid.o LifeGrid.moc.o utils.o life_gui.o
	$(CC) $(CFLAGS) $(QT_INCLUDE) $(QT_LIBS) -o $@ $^

life_gui.o : life_gui.cc LifeGrid.h
	$(CC) $(CFLAGS) $(QT_INCLUDE) $(QT_LIBS) -c life_gui.cc

LifeGrid.moc.o : LifeGrid.moc.cc
	$(CC) $(CFLAGS) $(QT_INCLUDE) $(QT_LIBS) -c LifeGrid.moc.cc

LifeGrid.moc.cc : LifeGrid.h
	moc-qt4 -o $@ $^

LifeGrid.o : LifeGrid.cc LifeGrid.h
	$(CC) $(CFLAGS) $(QT_INCLUDE) $(QT_LIBS) -c LifeGrid.cc

life : simulator.o miner_parser.tab.o lex.yy.o miner_type.o utils.o life.o
	$(CC) $(CFLAGS) -o $@ $^

utils.o : utils.cc utils.h
	$(CC) $(CFLAGS) -c utils.cc

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
	rm -f miner_parser.tab.c miner_parser.tab.h lex.yy.c *.o life life_gui LifeGrid.moc.cc
