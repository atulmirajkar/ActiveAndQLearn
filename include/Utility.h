#ifndef ATUL_UTILITY
#define ATUL_UTILITY

#include<iostream>
#include<stdlib.h>
#include<map>
#include<fstream>
#include<string>
#include<vector>
using namespace std;
class Utility{
public:
	static int getRandom(int range);
	static bool mySort(std::pair<int,double> ,std::pair<int,double>);
	static void tokenize(std::string str, std::string delimter, vector<string> & instance);
	static string  trim(string &s);
	static bool isStringNumber(const std::string inputString);
	static void toUpper(string & inputString);
};

#endif
