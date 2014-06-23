#include "Utility.h"

int Utility::getRandom(int range)
{
	return rand()%range;
}

bool Utility::mySort(std::pair<int,double> prob1,std::pair<int,double> prob2)
{
	return prob1.second > prob2.second;
}


string Utility::trim(string &s)
{
	size_t posBegin = s.find_first_not_of(" \t");
	size_t posEnd = s.find_last_not_of(" \t\n");
	size_t range = posEnd - posBegin +1;
	return s.substr(posBegin,range);
}


bool Utility::isStringNumber(const std::string inputString)
{
	char tempChar;
	for(int i=0;i<inputString.size();i++)
	{
		tempChar = inputString[i];
		if(tempChar<'0' || tempChar>'9')
		{
			return false;
		}
	}
	return true;
}

void Utility::tokenize(string str, string delimiter, vector<string> & instance)
{
	std::size_t pos=0;
	
	while((pos = str.find(delimiter.c_str()[0])) != string::npos)
	{
		instance.push_back(str.substr(0,pos));
		str.erase(0,pos+1);//1 for delimiter length
	}
	instance.push_back(str);
}
