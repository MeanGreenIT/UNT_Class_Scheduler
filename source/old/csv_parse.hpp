#ifndef CSV_PARSE_H
#define CSV_PARSE_H

#include "datarow.h"
#include <string>
#include <vector>

std::vector<DataRow*> ParseCSV(std::string filePath);
#endif