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
#define HOWMANYTOSHOW "howManytoShow"
#define UNTRAINEDEPISODES "unTrainedEpisodes"
#define ONLYPERCEPTION "onlyPerception"
#define CERTAINTYBUTNOASKS "certaintyButNoAsks"
using namespace std;

class QState{

	struct feature_node *x;
	double y;
	int certainity;
	double qValue[10][4];
	int wholeProblemIndex;
public:
	friend class QTable;
	
	QState()
	{
		x = NULL;
		y = -1;
		wholeProblemIndex = -1;
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
	QState* grid;
	int * nextStateCertainity;
	int numberOfCells;
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
	int numberOfAsks;
	int numberOfMisses;
	int howManytoShow;
	int unTrainedEpisodes;
	bool noAsks;
	bool useOnlyClassifier;
public:
	struct problem wholeProblem;

	string lrConfigPath;
	string wholeTrainingFile;
	QTable()
	{
		useVariableTempBool = false;
		displayTraversalBool = false;
		numberOfAsks = 0;
		numberOfMisses = 0;
		noAsks = false;
		useOnlyClassifier = false;
	}
	~QTable()
	{
	
	
       		free(wholeProblem.x[0]);
		free(wholeProblem.x);

		free(wholeProblem.y);
			
	}

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
	int getNumberOfMissedPicks();
	void makeAdjustmentToQTable(int currentCell,int tempCurrCertainity,int action,double tempval);
	friend class Test;

};	

#endif
