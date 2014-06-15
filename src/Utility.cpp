#include "Utility.h"

int Utility::getRandom(int range)
{
	return rand()%range;
}

bool Utility::mySort(std::pair<int,double> prob1,std::pair<int,double> prob2)
{
	return prob1.second < prob2.second;
}