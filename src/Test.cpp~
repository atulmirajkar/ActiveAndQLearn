#include "Test.h"

void Test::testAnneal(QTable & qt)
{
	
	for(int i=0;i<1000;i++)
	{
		double e = (double)i/1000;

		//max - (1-e)*(max-min);
		double first = 0.9 - (1-e)*(0.9 - 0.4);

		double second = 0.08 - (1-e)*(0.08 - 0.2);
		
		double third = 0.01 - (1-e)*(0.01 - 0.1);
		
		double fourth = 0.01 - (1-e)*(0.01 - 0.1);
		
		double fifth = 0 - (1-e)*(0 - 0.1);
		
		double sixth = 0 - (1-e)*(0 - 0.1);
		
		cout<<first<<"\t"<<second<<"\t"<<third<<"\t"<<fourth<<"\t"<<fifth<<"\t"<<sixth<<"\t"<<first + second + third + fourth + fifth + sixth<<endl;
	}

}

void Test::isLittleBigEndian()
{
	int test = 1;
	if(*((char *)&test) == 1)
	{
	       cout<<"Little Endian"<<endl;
	}
	else
	{
	       cout<<"Big Endian"<<endl;
	}
}
