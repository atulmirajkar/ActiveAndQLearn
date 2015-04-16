
#ifndef ATUL_RLSTRUCTURES
#define ATUL_RLSTRUCTURES

#include<fstream>
#include<iostream>
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
#define LRCONFIG "lrconfig"
#define WHOLETRAININGFILE "wholeTrainingFile"
#define TRAINSETFILESUBSET "trainSetFileSubset"
#define INITIALTRAININGDATACOUNT "initialTrainingDataCount"


extern ofstream myLog;
#define Malloc(type,n) (type *)malloc((n)*sizeof(type))

using namespace std;

class QStateBase{
public:
	struct feature_node *x;
	double y;
	int certainity;
//public:
	friend class QTableBase;

	QStateBase()
	{
		x = NULL;
		y = -1;

		certainity = 0;
	}
};


class QTableBase{
//protected:
public:
	struct problem wholeProblem;
	LRWrapper lr;
	int initialTrainingDataCount;
	string trainSetFileSubset;
	
	

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


		
//public:
	string lrConfigPath;
	string wholeTrainingFile;
	
	virtual void initiate()=0;
	virtual void readWholeProblem()=0;
	virtual void createPartialTrainingData()=0;
	virtual void newEpisode() = 0;
	virtual void displayQTable()=0;
	virtual void assignVariableTemp()=0;
        virtual void readConfig(char * configFilePath)=0;
};
#endif
