#ifndef ATUL_ACTIVE_AND_QLEARN
#define ATUL_ACTIVE_AND_QLEARN
#include<iostream>
#include<time.h>
#include<vector>
#include<math.h>
#include<algorithm>
using namespace std;

class QState{
	bool isCan;
	int certainity;
	double qValue[10][4];

public:
	friend class QTable;

	QState()
	{
		isCan = false;
		certainity = 0;
		for(int i=0;i<10;i++)
		{
			for(int j=0;j<4;j++)
			{
				qValue[i][j] = 0;		
			}
			
		}

	}
};

class QTable{

	QState* grid;
	int numberOfCells;
	double pdLRisCan[10]; //Prob Distri Logistic Reg returns a prob when there is a can
	double pdLRisnotCan[10]; 
	int episodeNumber;
	double exploitRate;
	double stepSize;
	double learningRate;
	int numberOfSteps;
public:
	QTable(int cells)
	{
		numberOfSteps = 0;
		learningRate=0.2;
		exploitRate = 0.6;
		episodeNumber = 1;
		stepSize = 0.9;
		numberOfCells = cells;
		
		grid = new QState[cells];
		
		pdLRisCan[0] = 0;
		pdLRisCan[1] = 0;
		pdLRisCan[2] = 0;
		pdLRisCan[3] = 0.05;
		pdLRisCan[4] = 0.05;
		pdLRisCan[5] = 0.1;
		pdLRisCan[6] = 0.1;
		pdLRisCan[7] = 0.1;
		pdLRisCan[8] = 0.2;
		pdLRisCan[9] = 0.4;

		pdLRisnotCan[0] = 0.4; 
		pdLRisnotCan[1] = 0.2; 
		pdLRisnotCan[2] = 0.1; 
		pdLRisnotCan[3] = 0.1; 
		pdLRisnotCan[4] = 0.1; 
		pdLRisnotCan[5] = 0.05; 
		pdLRisnotCan[6] = 0.05; 
		pdLRisnotCan[7] = 0; 
		pdLRisnotCan[8] = 0; 
		pdLRisnotCan[9] = 0; 

	}

	void newEpisode();
	void setLRCertainity();
	int getAction(int startCell);
	int getReward(int cellNo, int action);
	int getNextState(int currentCell,int action);
	void displayQTable();
};	

#endif
