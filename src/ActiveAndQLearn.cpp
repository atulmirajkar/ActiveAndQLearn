#include "ActiveAndQLearn.h"
#include"Utility.h"
#include "Test.h"
#include "LR.h"
#include <errno.h>

ofstream myLog;
#define Malloc(type,n) (type *)malloc((n)*sizeof(type))

int main()
{

	myLog.open("bin/log");
	QTable qTable;
	qTable.initiate();
	myLog.close();
	return 0;
}

void QTable::initiate()
{
		try{
		readConfig("src/config");

		//Test::testAnneal(qTable);
	
		//Test::isLittleBigEndian();
	        lr.lrConfigFile = lrConfigPath;
		lr.readLRConfig();
		lr.readMNISTData(wholeTrainingFile);

		readWholeProblem();
		createPartialTrainingData();
	        lr.LRTrain();
		//lr.LRTest();
		newEpisode();
		
	}
	catch(char * str)
	{
		cout<<str<<endl;
		myLog<<str<<endl;
		myLog.close();
       	}

}
// read in a problem (in libsvm format)
void QTable::readWholeProblem()
{
	int max_index, inst_max_index, i;
	long int elements, j;
	char *endptr;
	char *idx, *val, *label;

	char *filename = (char *)(this->wholeTrainingFile).c_str(); 
	FILE *fp = fopen(filename,"r");
	
	int max_line_len = 1024;
	char * line = Malloc(char,max_line_len);
       
	if(fp == NULL)
	{
		fprintf(stderr,"can't open input file %s\n",filename);
		exit(1);
	}


	wholeProblem.l = 0;
	elements = 0;
	while((line=Utility::readline(fp))!=NULL)
	{
		char *p = strtok(line," \t"); // label

		// features
		while(1)
		{
			p = strtok(NULL," \t");
			if(p == NULL || *p == '\n') // check '\n' as ' ' may be after the last feature
				break;
			elements++;
		}
		elements++; // for bias term
		wholeProblem.l++;
	}
	rewind(fp);
	
	//hardcoded bias
	wholeProblem.bias=0;

	wholeProblem.y = Malloc(double,wholeProblem.l);
	wholeProblem.x = Malloc(struct feature_node *,wholeProblem.l);
	//declared x_space locally instead of global
	struct feature_node *x_space;
	x_space = Malloc(struct feature_node,elements+wholeProblem.l);
	
	max_index = 0;
	j=0;
	for(i=0;i<wholeProblem.l;i++)
	{
		inst_max_index = 0; // strtol gives 0 if wrong format
		line = Utility::readline(fp);
		wholeProblem.x[i] = &x_space[j];
		label = strtok(line," \t\n");
		if(label == NULL) // empty line
			Utility::exit_input_error(i+1);

		wholeProblem.y[i] = strtod(label,&endptr);
		
		//added to know indices of choice value - start
		if(wholeProblem.y[i]==choiceValue)
		{
			positionsOfChoice.push_back(i);
		}
		//added to know indices of choice value - end

		if(endptr == label || *endptr != '\0')
			Utility::exit_input_error(i+1);

		while(1)
		{
			idx = strtok(NULL,":");
			val = strtok(NULL," \t");

			if(val == NULL)
				break;

			errno = 0;
			x_space[j].index = (int) strtol(idx,&endptr,10);
			if(endptr == idx || errno != 0 || *endptr != '\0' || x_space[j].index <= inst_max_index)
				Utility::exit_input_error(i+1);
			else
				inst_max_index = x_space[j].index;

			errno = 0;
			x_space[j].value = strtod(val,&endptr);
			if(endptr == val || errno != 0 || (*endptr != '\0' && !isspace(*endptr)))
				Utility::exit_input_error(i+1);

			++j;
		}

		if(inst_max_index > max_index)
			max_index = inst_max_index;

		if(wholeProblem.bias >= 0)
			x_space[j++].value = wholeProblem.bias;

		x_space[j++].index = -1;
	}

	if(wholeProblem.bias >= 0)
	{
		wholeProblem.n=max_index+1;
		for(i=1;i<wholeProblem.l;i++)
			(wholeProblem.x[i]-2)->index = wholeProblem.n;
		x_space[j-2].index = wholeProblem.n;
	}
	else
		wholeProblem.n=max_index;

	free(line);
	fclose(fp);
}



void QTable::readConfig(char * configFilePath)
{
	vector<string> tokenVector;
        ifstream config;
	map<string,string> configMapper;
	
	config.open(configFilePath);
	if(config.is_open())
	{
		string line="";
		while(getline(config,line))
		{
			line = Utility::trim(line);
			Utility::tokenize(line," ",tokenVector);
			if(tokenVector.size()!=2)
			{
				myLog<<"InValid config file<<endl";
				throw "Invalid config file";
			}
				

			configMapper[tokenVector[0].c_str()]=tokenVector[1];
			tokenVector.clear();
		}
		
	}
        config.close();
	
	if(configMapper.find(CEILING)==configMapper.end() || \
	   configMapper.find(MINTEMP)==configMapper.end() ||	\
	   configMapper.find(MAXTEMP)==configMapper.end() ||	\
	   configMapper.find(LEARNINGRATE)==configMapper.end() ||	\
	   configMapper.find(EXPLOITRATE)==configMapper.end() ||	\
	   configMapper.find(EPISODENUMBER)==configMapper.end() ||	\
	   configMapper.find(DISCOUNTFACTOR)==configMapper.end() ||		\
	   configMapper.find(NUMBEROFCELLS)==configMapper.end() ||
	   configMapper.find(USEVARIABLETEMP)==configMapper.end() ||	\
	   configMapper.find(DISPLAYTRAVERSALBOOL)==configMapper.end() ||	\
	   configMapper.find(CANDENSITY)==configMapper.end() ||  \
	   configMapper.find(LRCONFIG)==configMapper.end() || \
	   configMapper.find(WHOLETRAININGFILE)==configMapper.end() ||	\
	   configMapper.find(TRAINSETFILESUBSET)==configMapper.end() || \
	   configMapper.find(INITIALTRAININGDATACOUNT)==configMapper.end() || \
	   configMapper.find(CHOICEVALUE)==configMapper.end())
	{
		myLog<<"InValid config file<<endl";
		throw "Invalid config file";

	}
	
	ceiling = atoi(configMapper[CEILING].c_str());
	minTemp = atof(configMapper[MINTEMP].c_str());
	maxTemp = atof(configMapper[MAXTEMP].c_str());
	discountFactor = atof(configMapper[DISCOUNTFACTOR].c_str());
	exploitRate = atof(configMapper[EXPLOITRATE].c_str());
	episodeNumber = atoi(configMapper[EPISODENUMBER].c_str());
        learningRate = atof(configMapper[LEARNINGRATE].c_str());
	numberOfCells = atoi(configMapper[NUMBEROFCELLS].c_str());
	useVariableTempBool = atoi(configMapper[USEVARIABLETEMP].c_str());
	T = maxTemp;
	displayTraversalBool = atoi(configMapper[DISPLAYTRAVERSALBOOL].c_str());
	canDensity = atof(configMapper[CANDENSITY].c_str());
	grid = new QState[numberOfCells];
	nextStateCertainity = new int[numberOfCells];

	// min10 =  atof(configMapper[MIN10].c_str());
	// min9 = atof(configMapper[MIN9].c_str());
	// min8 = atof(configMapper[MIN8].c_str());
	// min7 = atof(configMapper[MIN7].c_str());
	// min6 = atof(configMapper[MIN6].c_str());
	// min5  = atof(configMapper[MIN5].c_str());
	// min4 = atof(configMapper[MIN4].c_str());



	// max10  = atof(configMapper[MAX10].c_str());
	// max9 =atof(configMapper[MAX9].c_str());
	// max8 = atof(configMapper[MAX8].c_str());
	// max7 = atof(configMapper[MAX7].c_str());
	// max6  = atof(configMapper[MAX6].c_str());
	// max5  = atof(configMapper[MAX5].c_str());
	// max4 = atof(configMapper[MAX4].c_str());


	lrConfigPath = configMapper[LRCONFIG];
	wholeTrainingFile = configMapper[WHOLETRAININGFILE];
	trainSetFileSubset = configMapper[TRAINSETFILESUBSET];
	initialTrainingDataCount = atoi(configMapper[INITIALTRAININGDATACOUNT].c_str());
	choiceValue = atoi(configMapper[CHOICEVALUE].c_str());
	//no need
	//assignPDLRCertainity();

}

void QTable::createPartialTrainingData()
{
	selectedData = new bool[wholeProblem.l];
	
	//initialize to false
	for(int i=0;i<wholeProblem.l;i++)
	{
		selectedData[i] = false;
	}
	srand(time(NULL));

	int tempIndex;
	ofstream trainLRFormat(trainSetFileSubset.c_str());
	
	for(int i=0;i<initialTrainingDataCount;i++)
	{
		tempIndex =Utility::getRandom(wholeProblem.l);
		selectedData[tempIndex] = true;
		trainLRFormat<<wholeProblem.y[tempIndex];
		
		for(int j=0;wholeProblem.x[tempIndex][j+1].index != -1;j++)
		{
			trainLRFormat<<" ";
			trainLRFormat<<wholeProblem.x[tempIndex][j].index<<":"<<wholeProblem.x[tempIndex][j].value;
		}
		trainLRFormat<<endl;
	}
	
}

// void QTable::assignPDLRCertainity(map<string,string> & configMapper)
// {

// 	pdLRisCan[0] = 0;
// 	pdLRisCan[1] = 0;
// 	pdLRisCan[2] = 0;
// 	pdLRisCan[3] = MIN4;
// 	pdLRisCan[4] = MIN5;
// 	pdLRisCan[5] = MIN6;
// 	pdLRisCan[6] = MIN7;
// 	pdLRisCan[7] = MIN8;
// 	pdLRisCan[8] = MIN9;
// 	pdLRisCan[9] = MIN10;

// 	pdLRisnotCan[0] = MIN10;
// 	pdLRisnotCan[1] = MIN9;
// 	pdLRisnotCan[2] = MIN8;
// 	pdLRisnotCan[3] = MIN7;
// 	pdLRisnotCan[4] = MIN6;
// 	pdLRisnotCan[5] = MIN5;
// 	pdLRisnotCan[6] = MIN4;
// 	pdLRisnotCan[7] = 0; 
// 	pdLRisnotCan[8] = 0; 
// 	pdLRisnotCan[9] = 0; 


// }


// void QTable::assignPDLRCertainity()
// {

// 	double e = (double)episodeNumber/ceiling;

// 	//max - (1-e)*(max-min);
	
// 	pdLRisCan[0] = 0;
// 	pdLRisCan[1] = 0;
// 	pdLRisCan[2] = 0;
// 	pdLRisCan[3] = max4 - (1-e)*(max4-min4);
// 	pdLRisCan[4] = max5 - (1-e)*(max5-min5);
// 	pdLRisCan[5] = max6 - (1-e)*(max6-min6);
// 	pdLRisCan[6] = max7 - (1-e)*(max7-min7);
// 	pdLRisCan[7] = max8 - (1-e)*(max8-min8);
// 	pdLRisCan[8] = max9 - (1-e)*(max9-min9);
// 	pdLRisCan[9] = max10 - (1-e)*(max10-min10);

// 	pdLRisnotCan[0] = max10 - (1-e)*(max10-min10);
// 	pdLRisnotCan[1] = max9 - (1-e)*(max9-min9); 
// 	pdLRisnotCan[2] = max8 - (1-e)*(max8-min8);
// 	pdLRisnotCan[3] = max7 - (1-e)*(max7-min7);
// 	pdLRisnotCan[4] = max6 - (1-e)*(max6-min6);
// 	pdLRisnotCan[5] = max5 - (1-e)*(max5-min5);
// 	pdLRisnotCan[6] = max4 - (1-e)*(max4-min4);
// 	pdLRisnotCan[7] = 0; 
// 	pdLRisnotCan[8] = 0; 
// 	pdLRisnotCan[9] = 0; 


// }


void QTable::newEpisode()
{
	int averageReward = 0;
	int tempCurrCertainity = 0;
	int tempNextCertainity = 0;

	//set 10% of cells to have cans at random
	while(true)
	{
		srand(episodeNumber + time(NULL));
		averageReward = 0;

		//probability distribution function of episodes
		//assignPDLRCertainity();

		//clear the grid from cans
		clearGrid();

		//scatter random cans for the next episode
		scatterRandomCans();

		//set certainity values to cell with a probability distribution
		setLRCertainity();
		
		if(displayTraversalBool)
		{
			//logging
			for(int i=0;i<numberOfCells;i++)
			{
				myLog<<grid[i].y<<" "<<grid[i].certainity<<endl; 
			}
			myLog<<endl;
		
			//logging
		}
		//restart at cell 0
		int currentCell = 0;
		int currentCertainity = 0;
		while(true)
		{
			currentCertainity = grid[currentCell].certainity;
			int action = getAction(currentCell,currentCertainity);

			
			int reward = getReward(currentCell,currentCertainity, action);
			
			averageReward +=reward;

			//assigns reward and also the nextStateCertainity
			//if action is pick up the current  x and  y values are also changed
			int nextState = getNextState(currentCell,currentCertainity,action);
		
			if(displayTraversalBool)
			{
				displayTraversal(currentCell,currentCertainity,action,nextState,grid[nextState].certainity,grid[currentCell].y,grid[nextState].y);
	
			}

		
			int nextAction = getAction(nextState,grid[nextState].certainity);
               
			//Use currentCertainity as grid[currentCell].certainity may have changed
           		tempCurrCertainity = currentCertainity;
			tempNextCertainity = grid[nextState].certainity;
                        //tempNextCertainity = nextStateCertainity[nextState];
			//check for out of bounds
			if(currentCell<0 || currentCell>numberOfCells-1 || tempCurrCertainity <1 || tempCurrCertainity>10 ||action<1 || action>4 || nextState<0 || nextState>numberOfCells-1 || tempNextCertainity <1 || tempNextCertainity>10 || nextAction<1 || nextAction>4)
			{
				myLog<<"Out of bounds Array"<<endl;
				//logging
				myLog<<"Action Returned "<<action-1<<endl;
				//logging
				//logging
				myLog<<"reward Returned "<<reward<<endl;
				//logging
				// logging
				myLog<<"next state  Returned "<<nextState-1<<endl;
				//logging
				// logging
				myLog<<"next Action Returned "<<nextAction-1<<endl;
				//logging
				myLog<<"current cell "<<currentCell<<endl;
				myLog<<"tempCurrCertainity "<<tempCurrCertainity<<endl;
				myLog<<"tempNextCertainity "<<tempNextCertainity<<endl;
				displayQTable();
				return;
			}
			double tempVal= ((1-learningRate) * grid[currentCell].qValue[tempCurrCertainity-1][action-1]) + (learningRate* (reward+discountFactor*(grid[nextState].qValue[tempNextCertainity-1][nextAction-1])));
	
			// //logging
			// myLog<<"Q val assigned "<<tempVal<<endl;
			// //logging
			grid[currentCell].qValue[tempCurrCertainity-1][action-1] = tempVal;

		// 	//logging
// 			myLog<<"Q val assigned "<<grid[currentCell].qValue[tempCurrCertainity][action-1]<<endl;
// 			myLog<<"cell no "<<currentCell<<endl;
// 			myLog<<"cell certainity "<<grid[currentCell].certainity<<endl;
// //logging
			
		

			currentCell = nextState;
                       
			if(currentCell==numberOfCells-1)
			{
				break;
			}
		

			numberOfSteps++;
		}//end episode
		//logging
		myLog<<episodeNumber<<"\t"<<numberOfSteps<<"\t"<<averageReward<<"\t"<<T<<endl;
		// myLog<<"Number of steps "<<numberOfSteps<<endl;
		// myLog<<"Average Reward "<<averageReward<<endl;
		// myLog<<"Current Temperature "<<T<<endl;
//logging
		numberOfSteps = 0;
		episodeNumber++;
		if(displayTraversalBool)
		{
			displayQTable();
		}
		if(useVariableTempBool)
		{
			assignVariableTemp();
		}
		if(episodeNumber==ceiling)
		{
			displayTraversalBool = true;
			myLog<<endl;
			displayQTable();
			myLog<<endl;
			T=minTemp;
			myLog<<endl<<"temp "<<T<<endl;

		}

		if(episodeNumber==ceiling+1)
		{
			//logging
			for(int i=0;i<numberOfCells;i++)
			{
				myLog<<grid[i].y<<" "<<grid[i].certainity<<endl; 
			}
			myLog<<endl;
			
			//logging
		
			
			break;
		}
	}//repeat for next episode
}

void QTable::displayTraversal(int currentState,int currentCertainity, int action,int nextState,int nextStateCertainity,int currentVal,int nextVal)
{

	if(action == 1)//left
	{
		myLog<<"Travelling from state ("<<currentState<<" "<<currentVal<<" "<<currentCertainity<<") taking action left - to state ("<<nextState<<" "<<nextVal<<" "<<nextStateCertainity<<")"<<endl; 
	}
	else if(action==2)//right
	{
		myLog<<"Travelling from state ("<<currentState<<" "<<currentVal<<" "<<currentCertainity<<") taking action right - to state ("<<nextState<<" "<<nextVal<<" "<<nextStateCertainity<<")"<<endl; 

	}
	else if(action==3)//pickup
	{
		myLog<<"Travelling from state ("<<currentState<<" "<<"c"<<" "<<currentCertainity<<") taking action pickup - to state ("<<nextState<<" "<<nextVal<<" "<<nextStateCertainity<<")"<<endl; 

	}
	else if(action==4)//ask
	{
		myLog<<"Travelling from state ("<<currentState<<" "<<currentVal<<" "<<currentCertainity<<") taking action ask - to state ("<<nextState<<" "<<nextVal<<" "<<nextStateCertainity<<")"<<endl; 

	}
}
void QTable::scatterRandomCans()
{
	int tempChoiceRandom=0;
	int tempRandom=0;
	// for(int i=0;i<(canDensity*numberOfCells);i++)
	// {
	// 	tempRandom =Utility::getRandom(numberOfCells); 
		
	// 	//logging
	// 	// myLog<<"Random "<<tempRandom<<endl;
	// 	//logging
			
	// 	if(!grid[tempRandom].isCan)
	// 	{
	// 		grid[tempRandom].isCan = true;
	// 	}
	// 	else
	// 	{
	// 		i--;
	// 	}
	       
	// }

	for(int i=0;i<(canDensity*numberOfCells);i++)
	{
	        tempChoiceRandom =Utility::getRandom(positionsOfChoice.size()); 
		//check if already selected or not
		if(selectedData[tempChoiceRandom])
		{
			i--;
			continue;
		}
	        tempRandom =Utility::getRandom(numberOfCells); 
		
		if(grid[tempRandom].x!=NULL)
		{
			i--;
			continue;
		
		}
		//get indices from positionOfChoice
		grid[tempRandom].x = wholeProblem.x[positionsOfChoice[tempChoiceRandom]];
		grid[tempRandom].y = wholeProblem.y[positionsOfChoice[tempChoiceRandom]];
              
	}

	//fill rest of the cells
	for(int i=0;i<numberOfCells;i++)
	{
		if(grid[i].x!=NULL)
		{
			continue;
		
		}
		
		getRandomImageIndexWhole(&tempRandom);
		//get indices from whole problem itself
		grid[i].x = wholeProblem.x[tempRandom];
		grid[i].y = wholeProblem.y[tempRandom];

	}
}

void QTable::getRandomImageIndexWhole(int * tempRandom)
{
	while(true)
	{
		*tempRandom =Utility::getRandom(wholeProblem.l);
		if(selectedData[*tempRandom])
		{
			continue;
		}
		else
		{
			//value in tempRandom is nit in selectedData
			return;
		}
		
	}

}

void QTable::clearGrid()
{
	//reset cans to 0
		
	// for(int i=0;i<numberOfCells;i++)
	// {
	// 	grid[i].isCan = false;
	// }

	for(int i=0;i<numberOfCells;i++)
	{
		grid[i].x = NULL;
		grid[i].y = -1;
	}
}

int QTable::getNextState(int currentCell,int currentCertainity,int action)
{
	//if going off grid retrun to the same state
	if(currentCell==0 && action==1)
	{

		return currentCell;
	}
	
	if(action==1)//if left
	{

		return currentCell-1;
	}
	else if(action==2)//if right
	{

		return currentCell+1;
	}
	
	//if action is pickup check ground truth and set certainity
	else if(action==3)//if pickup
	{
		// if(grid[currentCell].y==choiceValue)
		// {
		// 	grid[currentCell].certainity = 10;
		// }
		// else
		// {
		// 	grid[currentCell].certainity = 1;

		// }

		
		//add to training set and train model
		
		// replace currentcell with random image
		int tempRandom = -1;
		getRandomImageIndexWhole(&tempRandom);
		grid[currentCell].x = wholeProblem.x[tempRandom];
		grid[currentCell].y = wholeProblem.y[tempRandom];

		//get certainity of new current cell configuration
		grid[currentCell].certainity = (int) (lr.predictProbabiltiyWrapper(grid[currentCell].x,choiceValue) * 10) + 1;

		return currentCell;
	}
	
	else if(action==4)//if ask
	{
		if(grid[currentCell].y==choiceValue)
		{
			grid[currentCell].certainity = 10;
		}
		else
		{
			grid[currentCell].certainity = 1;

		}

		
		return currentCell;
	}
}

int QTable::getReward(int cellNo,int currentCertainity, int action)
{

	//reward depending on grid
	// if(cellNo==0 && action==1)
	// {
	// 	return -20;
	// }
	// if(cellNo==numberOfCells-2 && action==2)
	// {
	// 	return 20;
	// }

	//reward for going left or right
	if(action==1)//left
	{
		return -2;
	}
	else if(action==2)//right
	{
		return -2;
	}
	else if(action==3)//pickup
	{
		//look at the ground truth

		//if(grid[cellNo].isCan)
		if(grid[cellNo].y==choiceValue)
		{
			//grid[cellNo].isCan = false;
			
			return 15;
		}
		else
		{
			return -15;
		}
	}
	else if(action==4)//ask
	{
		return -4;
	}

	

	//reward depending on certainity
	// int certainityVal = currentCertainity;
	
        // //certainity of no can high
	// if(certainityVal>=1 && certainityVal<=3)
	// {
	//         if(action==3)//pickup
	// 	{
	// 		return -7;
	// 	}
	// 	else if(action==4)//ask
	// 	{
	// 		return -5;
	// 	}
	// 	if(action==1)//left
	// 	{
	// 		return -2;
	// 	}
	// 	else if(action==2)//right
	// 	{
	// 		return -2;
	// 	}


	// }
	// //uncertainity higher
	// else if(certainityVal>=4 && certainityVal<=7)
	// {
	//         if(action==3)//pickup
	// 	{
	// 		return -5;
	// 	}
	// 	else if(action==4)//ask
	// 	{
	// 		return -1;
	// 	}
	// 	if(action==1)//left
	// 	{
	// 		return -2;
	// 	}
	// 	else if(action==2)//right
	// 	{
	// 		return -2;
	// 	}
	// }

	// //certainity of can being there is high
	// else if(certainityVal>=8 && certainityVal<=10)
	// {
	// 	if(action==3)//pickup
	// 	{
	// 		return -1;
	// 	}
	// 	else if(action==4)//ask
	// 	{
	// 		return -5;
	// 	}
	// 	if(action==1)//left
	// 	{
	// 		return -2;
	// 	}
	// 	else if(action==2)//right
	// 	{
	// 		return -2;
	// 	}
	// }
     
}

void QTable::assignVariableTemp()
{

	if(episodeNumber>=ceiling)
	{
		T = minTemp; 
	}
	else
	{
		double e = (double)episodeNumber/ceiling;
		T= minTemp + (1-e)*(maxTemp - minTemp);
	}
}

int QTable::getAction(int cellNo,int currentCertainity)
{
	//0 - left
	//1 - right
	//2  - pickup
	//3 - ask
	//vector<pair<action,probability>>
	vector< pair<int,long double> > actionProbVector;
	vector< pair<int,long double> > qValueVector;
	double numerator = 0;
	double denominator=0;
	int tempCertainity =0;
	double tempNumerator = 0;
	//sort existing q -values for given cell and certainity
	for(int i=0;i<4;i++)
	{
		tempCertainity = currentCertainity; 
		qValueVector.push_back(make_pair(i,grid[cellNo].qValue[tempCertainity-1][i]));
	}
	
	sort(qValueVector.begin(),qValueVector.end(),Utility::mySort);
	
        //logging
	// myLog<<"sorted qValueVector"<<endl;
	// for(int i=0;i<4;i++)
	// {
	// 	myLog<<qValueVector[i].first<<"\t"<<qValueVector[i].second<<endl;
	// }
	// myLog<<endl;
	//logging

	// //logging
	// myLog<<"Current Temp "<<T<<endl;
	// //logging
	for(int i=0;i<qValueVector.size();i++)
	{ 
		tempNumerator = double(qValueVector[i].second - qValueVector[0].second)/double(this->T);
		//tempNumerator = double(qValueVector[i].second)/double(this->T);
		if(exp(tempNumerator)>=pow(2,sizeof(double)*2)-1)
		{
			return -1;
		}
		numerator = exp(tempNumerator);
		
	       	actionProbVector.push_back(make_pair(qValueVector[i].first,numerator));
		denominator += numerator; 
	}
	// for(int i=0;i<4;i++)
	// {
	  	
	// 	tempCertainity = grid[cellNo].certainity;
	// 	// //logging
	// 	// myLog<<"cell No "<<"\t"<<cellNo<<endl;
	// 	// myLog<<"certaintiy"<<"\t"<<tempCertainity<<endl;
	// 	// myLog<<"Q Val for above "<<"\t"<<grid[cellNo].qValue[tempCertainity-1][i]<<endl;
	// 	// myLog<<"exploitRate "<<exploitRate<<endl;
	// 	// myLog<<"episode number "<<episodeNumber<<endl;
	// 	//logging
	// 	numerator = exp(double(exploitRate * episodeNumber * grid[cellNo].qValue[tempCertainity-1][i]));
	// 	actionProbVector.push_back(make_pair(i,numerator));
	// 	denominator += numerator; 
	// 	//logging
	// 	myLog<<"Numberator"<<"\t"<<numerator<<endl;
	// 	//logging
		
	// }
	
	
	for(int i=0;i<4;i++)
	{
		actionProbVector[i].second/=denominator;
	}

	sort(actionProbVector.begin(),actionProbVector.end(), Utility::mySort);
	
        // //logging
	// myLog<<"sortedVector"<<endl;
	// for(int i=0;i<4;i++)
	// {
	// 	myLog<<actionProbVector[i].first<<"\t"<<actionProbVector[i].second<<endl;
	// }
	// myLog<<endl;
	// //logging
	double random = ((double)rand() / RAND_MAX);
	double total=0;
	for(int i=0;i<4;i++)
	{
		if(random<=actionProbVector[i].second + total)
		{
			return actionProbVector[i].first+1;
		}
		else
		{
			total +=  actionProbVector[i].second;
		}

	}
	
}

void QTable::setLRCertainity()
{
	double random= 0;
	double total=0;
	// for(int i=0;i<numberOfCells;i++)
	// {
	// 	random = ((double)rand() / RAND_MAX);
	// 	/*//logging
	// 	myLog<<random<<endl;
	// 	//logging
	// 	*/
	// 	if(grid[i].isCan)
	// 	{
	// 		total = 0;
	// 		for(int j=9;j>=0;j--)
	// 		{
	// 			if(random<=pdLRisCan[j] + total)
	// 			{
	// 				grid[i].certainity = j+1;
	// 				break;
	// 			}
	// 			else
	// 			{
	// 				total +=  pdLRisCan[j];
	// 			}
				
	// 		}
	// 	}

	// 	else if(!grid[i].isCan)
	// 	{
	// 		total = 0;
	// 		for(int j=0;j<10;j++)
	// 		{
	// 			if(random<=pdLRisnotCan[j]+total)
	// 			{
	// 				grid[i].certainity =j+1;
	// 				break;
	// 			}
	// 			else
	// 			{
	// 				total +=pdLRisnotCan[j];
	// 			}
	// 		}
	// 	}
	// }
	for(int i=0;i<numberOfCells;i++)
	{
		cout<<"Actual Label"<<grid[i].y;
		grid[i].certainity = (int) (lr.predictProbabiltiyWrapper(grid[i].x,choiceValue) * 10) + 1;
		
	}
	
}

void QTable::displayQTable()
{
	myLog<<"QTABLE"<<endl;
	for(int i=0;i<numberOfCells;i++)
	{
		myLog<<"cell "<<i<<endl;
		for(int j=0;j<10;j++)
		{
			//myLog<<"cell "<<i<<" certainity "<<j<<endl;
			for(int k=0;k<4;k++)
			{
				//myLog<<grid[i].qValue[grid[i].certainity][k]<<"\t";
				myLog<<grid[i].qValue[j][k]<<"     ";
			}
			myLog<<endl;
		}
	}
}
