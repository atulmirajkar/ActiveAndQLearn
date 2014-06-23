#ifndef ATUL_ACTIVE_AND_QLEARN
#define ATUL_ACTIVE_AND_QLEARN
#include<iostream>
#include<fstream>
#include<string>
#include<time.h>
#include<vector>
#include<math.h>
#include<map>
#include<stdlib.h>
#include<algorithm>

#define CEILING "ceiling"
#define MINTEMP "minTemp"
#define MAXTEMP "maxTemp"
#define LEARNINGRATE "learningRate"
#define EXPLOITRATE "exploitRate" 
#define EPISODENUMBER "episodeNumber"
#define DISCOUNTFACTOR "discountFactor"
#define NUMBEROFCELLS "numberOfCells"
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
	int * nextStateCertainity;
	int numberOfCells;
	double pdLRisCan[10]; //Prob Distri Logistic Reg returns a prob when there is a can
	double pdLRisnotCan[10]; 
	int episodeNumber;
	double exploitRate;
	double learningRate;
	double discountFactor;
	int numberOfSteps;
	double T;
	double minTemp;
	double maxTemp;
	int ceiling;
public:
	QTable()
	{
		//grid = new QState[cells];
		//nextStateCertainity = new int[cells];

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
	/* QTable(int cells) */
	/* { */
	/* 	T=1; */
	/* 	numberOfCells = cells; */
       	/* } */

	void newEpisode();
	void setLRCertainity();
	int getAction(int startCell,int currentCertainity);
	int getReward(int cellNo,int currentCertainity, int action);
	int getNextState(int currentCell,int currentCertainity,int action);
	void displayQTable();
	void clearGrid();
	void scatterRandomCans();
	void displayTraversal(int currentState,int currentCertainity, int action,int nextState,int nextStateCertainity);
	void assignVariableTemp();
	void readConfig(char * configFilePath);
};	

#endif
