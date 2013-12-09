#ifndef MINER_PARSER_H
#define MINER_PARSER_H
#include <string>
#include "miner_type.h"
#include "miner_parser.tab.h"

/*
 * Parse a miner file stored in sin
 * NOT threaded safe, reasons are in .cc file
 */
std::pair<std::string, Struct> parse_miner_string(std::string &sin);
#endif /* !MINER_PARSER_H */

