#include "csv_parse.h"
#include "datarow.h"
#include <algorithm>
#include <cctype>
#include <iostream>
#include <locale>
#include <fstream>
#include <string>
#include <vector>

// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

// trim from start (copying)
static inline std::string ltrim_copy(std::string s) {
    ltrim(s);
    return s;
}

// trim from end (copying)
static inline std::string rtrim_copy(std::string s) {
    rtrim(s);
    return s;
}

// trim from both ends (copying)
static inline std::string trim_copy(std::string s) {
    trim(s);
    return s;
}

std::vector<DataRow*> ParseCSV(std::string filePath)
{
	std::vector<DataRow*> finalTable;

	std::string line;
	std::vector<std::string> result;
	std::ifstream csvFile(filePath);
	int i = 0;
	while(std::getline(csvFile, line))
	{
		//std::cout << line << std::endl;
		std::vector<std::string> tempVector;
		int pos = 0;
		if(i>0){
			while((pos = line.find(",")) != std::string::npos)
			{
				std::cout << pos << " :" <<line.substr(0,pos) << std::endl;
				tempVector.push_back(line.substr(0,pos));
				line.erase(0,pos+1);
			}
			std::cout << line << std::endl;
			std::cout << tempVector.size() << std::endl;
			tempVector.push_back(line);
			for(int j = 0; j < tempVector.size();j++)
			{
				if(tempVector[j]=="")
					tempVector[j] = "!NA";
				trim(tempVector[j]);
			}
			finalTable.push_back(new DataRow(
				std::to_string(i),
				tempVector[0],
				tempVector[1],
				tempVector[2],
				tempVector[3],
				tempVector[4],
				tempVector[5],
				tempVector[7],
				tempVector[8],
				tempVector[9],
				tempVector[10],
				tempVector[11]
				));
			for(int j = 0; j < tempVector.size(); j++)
			{
				std::cout << j <<":  " << tempVector[j] << std::endl;
			}
		}
		i++;
		std::cout<<i<<std::endl;
	}

	std::cout << "Return final table\n";
	return finalTable;
}