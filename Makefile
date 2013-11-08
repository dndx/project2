.PHONY clean

miner_parser.tab.c : miner_parser.y miner_type.h
	bison -d miner_parser.y

lex.yy.c : miner_type.h miner_parser.tab.c
	flex --noyyunput miner_parser.l

clean :
	rm -f miner_parser_tab.c miner_parser.tab.h lex.yy.c
