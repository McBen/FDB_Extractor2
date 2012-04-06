/** 
based on:
http://www.zedwood.com/article/112/cpp-csv-parser

first tried:
http://code.google.com/p/csv-parser-cplusplus/
but its bugged
 
**/
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#ifndef CSV_PARSER_HPP_INCLUDED

bool csvparse(std::ifstream& in, std::vector<std::string>& row);

#endif 
