#ifndef ATUL_ACTIVE_AND_QLEARN_DRIVE
#define ATUL_ACTIVE_AND_QLEARN_DRIVE
#include<iostream>
#include "RLStructures.h"
#include "car_graphics.h"
#include "sstream"

#define NUMBER_OF_CLASSES 3
//LEFT RIGHT STAY
#define NUMBER_OF_ACTIONS 4
#define MY_CAR_STATES 5
#define OTHER_CAR_STATES 3
#define DRIVE_LENGTH 400
#define DRIVE_WIDTH 150
#define CARSTEPSIZE "carstepsize"
#define CHOICEVALUE_EGG "choicevalueegg"
#define CHOICEVALUE_OTHERCAR "choicevalueothercar"
#define WINDOWFRAMERATE "windowframerate"
#define CHOICEVALUE_REST "choicevaluerest"
#define IMAGES_FOLDER "imagesFolder"
#define CAR_DENSITY "carDensity"
#define MAX_NUMBER_OF_DRAWABLE_ITEMS 3
#define NO_STEPS_IN_EPISODE "nosInEpisode"
#define UNTRAINED_EPIDODES "untrainedEpisodes"
#define QTABLEFILE "qtableFile"
#define READQTABLE "readQtable"
//constants for mapping actual distances to QTable - grid indexes
#define MAP_MYCAR_X 0
#define MAP_OTHERCAR_X 1
#define MAP_OTHERCAR_Y 2


//constants for actions
#define ACTION_LEFT 1
#define ACTION_RIGHT 2
#define ACTION_STAY 3
#define ACTION_ASK 4

//constants for classes
#define CLASS_OTHERCAR 0
#define CLASS_EGG 1
#define CLASS_REST 2



using namespace std;



class QStateDrive: public QStateBase{
private:
	double qValue[10][NUMBER_OF_ACTIONS];
	
public:
	friend class QTableDrive;

	QStateDrive()
	{

		for(int i=0;i<10;i++)
		{
			for(int j=0;j<NUMBER_OF_ACTIONS;j++)
			{
				qValue[i][j] = 0;		
			}
		}
		
	}

	
};




class QTableDrive: public QTableBase{
private:
	
	//double canDensity;
	QStateDrive **** grid;
	bool * selectedData;
	int nosInEpisode;
	int untrainedEpisodes;
	//SIMULATION
	int carStepSize;
	int choiceValEgg;
	int choiceValOtherCar;
	int choiceValRest;
	vector<int> eggIndexes;
	vector<int> otherCarIndexes;
	vector<int> restIndexes;
     	Simulation simulation;
	
public:


	QTableDrive()
	{
		
	}
	~QTableDrive()
	{
	
		//delete grid
		
	}
	
	void readConfig(char * configFilePath);
	void initiate();


	void newEpisode();

	void displayQTable();
	void readWholeProblem();
	void createPartialTrainingData();
	void assignVariableTemp();

	
	//specific
	struct feature_node * getImageWithCurrentView(int ,int ,int ,int*);
	double compareClassesAndGetClass(struct feature_node * x,int * classLabel);
        int getAction(int myCarX,int otherX, int otherY, int classLabel, int certainty);
	int mapActualDistToIndex(int ,int );
	int getReward(int,int * carCollisions, int * eggsCollected );
	void doAction(int action,int *,int *,int );
	void displayTraversal(int myCarCurrX,int otherCurrX,int otherCurrY,int currClass,int currCertainty,int currAction,int next_myCarCurrX,int next_otherCurrX,int next_otherCurrY,int next_currClass,int next_currCertainty);
	void readQTableFromFile(string );
	friend class Test;
};	



#endif
