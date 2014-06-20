#ifndef ATUL_UTILITY
#define ATUL_UTILITY

#include<iostream>
#include<stdlib.h>
#include<map>
#include<fstream>
class Utility{
public:
	static int getRandom(int range);
	static bool mySort(std::pair<int,double> ,std::pair<int,double>);
};

#endif
