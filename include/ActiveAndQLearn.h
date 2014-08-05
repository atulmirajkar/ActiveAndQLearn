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
#include "linear.h"
#include "LR.h"

#define CEILING "ceiling"
#define MINTEMP "minTemp"
#define MAXTEMP "maxTemp"
#define LEARNINGRATE "learningRate"
#define EXPLOITRATE "exploitRate" 
#define EPISODENUMBER "episodeNumber"
#define DISCOUNTFACTOR "discountFactor"
#define NUMBEROFCELLS "numberOfCells"
#define USEVARIABLETEMP "useVariableTemp"
#define DISPLAYTRAVERSALBOOL "displayTraversalBool"
#define CANDENSITY "canDensity"
#define LRCONFIG "lrconfig"
#define WHOLETRAININGFILE "wholeTrainingFile"
#define TRAINSETFILESUBSET "trainSetFileSubset"
#define INITIALTRAININGDATACOUNT "initialTrainingDataCount"
#define CHOICEVALUE "choiceValue"

/* #define MIN10 "min10"  */
/* #define MAX10 "max10"  */
/* #define MIN9 "min9" */
/* #define MAX9 "max9" */
/* #define  MIN8 "min8" */
/* #define  MAX8 "max8" */
/* #define  MIN7 "min7"
#define  MAX7 "max7"
#define  MIN6 "min6"
#define  MAX6 "max6"
#define  MIN5 "min5"
#define  MAX5 "max5"
#define  MIN4 "min4"
#define  MAX4 "max4"
*/
using namespace std;

class QState{
	//bool isCan;
	struct feature_node *x;
	double y;
	int certainity;
	double qValue[10][4];

public:
	friend class QTable;

	QState()
	{
		x = NULL;
		y = -1;
		//isCan = false;
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
	LRWrapper lr;
	int initialTrainingDataCount;
	bool * selectedData;
	int choiceValue;
	vector<int> positionsOfChoice;
	string trainSetFileSubset;
	struct problem wholeProblem;
	QState* grid;
	int * nextStateCertainity;
	int numberOfCells;
	//double pdLRisCan[10]; //Prob Distri Logistic Reg returns a prob when there is a can
	//double pdLRisnotCan[10]; 
	int episodeNumber;
	double exploitRate;
	double learningRate;
	double discountFactor;
	int numberOfSteps;
	double T;
	double minTemp;
	double maxTemp;
	int ceiling;
	bool useVariableTempBool;
	bool displayTraversalBool;
	double canDensity;
	
	/* double min10; */
	/* double max10; */
       	/* double min9; */
	/* double max9; */
	/* double min8; */
	/* double max8; */
	/* double min7; */
	/* double max7; */
	/* double min6; */
	/* double max6; */
	/* double min5; */
	/* double max5; */
	/* double min4; */
	/* double max4; */
	
       
public:
	;

	string lrConfigPath;
	string wholeTrainingFile;
	QTable()
	{
		useVariableTempBool = false;
		displayTraversalBool = false;
	}
	~QTable()
	{
	
	
       		free(wholeProblem.x[0]);
		free(wholeProblem.x);

		free(wholeProblem.y);
			
	}
	/* QTable(int cells) */
	/* { */
	/* 	T=1; */
	/* 	numberOfCells = cells; */
       	/* } */

	void getRandomImageIndexWhole(int * tempRandom);
	void initiate();
	void readWholeProblem();
	void createPartialTrainingData();
	void newEpisode();
	void setLRCertainity();
	int getAction(int startCell,int currentCertainity);
	int getReward(int cellNo,int currentCertainity, int action);
	int getNextState(int currentCell,int currentCertainity,int action);
	void displayQTable();
	void clearGrid();
	void scatterRandomCans();
	void displayTraversal(int currentState,int currentCertainity, int action,int nextState,int nextStateCertainity,int currentVal,int nextVal);
	void assignVariableTemp();
	void readConfig(char * configFilePath);
	//void assignPDLRCertainity(map<string,string> & configMapper);
	//void assignPDLRCertainity();
	friend class Test;
};	

#endif
