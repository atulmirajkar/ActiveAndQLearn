#include "ActiveAndQLearnDrive.h"



void QTableDrive::initiate()
{

	try{

		//read config and initialize grid[myx][otherx][othery][class]
		readConfig("src/configDrive");
		
		//read config for logistic regression
	        lr.lrConfigFile = lrConfigPath;
		lr.readLRConfig();

		
                /*
		  read the binary file with MNIST data
		  write label and file sparse vector to whole train file
		*/
		lr.readMNISTData(wholeTrainingFile,choiceValEgg,choiceValOtherCar,choiceValRest);
		
		/*
		  populate struct problem wholeProblem
		  wholeProblem.l
		  wholeProble.x
		  wholeProblem.y
		  populate x
		 */
		readWholeProblem();
		
		
		/*
		  populate trainsetfilesubset
		*/
		createPartialTrainingData();
		
	        lr.LRTrain(true);
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



void QTableDrive::readConfig(char * configFilePath)
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
	
	if(configMapper.find(CEILING)==configMapper.end() ||	\
	   configMapper.find(MINTEMP)==configMapper.end() ||	\
	   configMapper.find(MAXTEMP)==configMapper.end() ||		\
	   configMapper.find(LEARNINGRATE)==configMapper.end() ||	\
	   configMapper.find(EXPLOITRATE)==configMapper.end() ||	\
	   configMapper.find(EPISODENUMBER)==configMapper.end() ||	\
	   configMapper.find(DISCOUNTFACTOR)==configMapper.end() ||	\
	   configMapper.find(USEVARIABLETEMP)==configMapper.end() ||	\
	   configMapper.find(DISPLAYTRAVERSALBOOL)==configMapper.end() || \
       	   configMapper.find(LRCONFIG)==configMapper.end() ||		\
	   configMapper.find(WHOLETRAININGFILE)==configMapper.end() ||	\
	   configMapper.find(TRAINSETFILESUBSET)==configMapper.end() || \
	   configMapper.find(CARSTEPSIZE)==configMapper.end() ||	\
	   configMapper.find(CHOICEVALUE_EGG)==configMapper.end() ||	\
	   configMapper.find(CHOICEVALUE_OTHERCAR)==configMapper.end() || \
	   configMapper.find(CHOICEVALUE_REST)==configMapper.end() ||	\
      	   configMapper.find(WINDOWFRAMERATE)==configMapper.end() ||	\
	   configMapper.find(IMAGES_FOLDER)==configMapper.end() ||	\
	   configMapper.find(CAR_DENSITY)==configMapper.end() ||	\
	   configMapper.find(NO_STEPS_IN_EPISODE)==configMapper.end() ||	\
	   configMapper.find(UNTRAINED_EPIDODES)==configMapper.end() || \
       	   configMapper.find(INITIALTRAININGDATACOUNT)==configMapper.end())
		
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
	useVariableTempBool = atoi(configMapper[USEVARIABLETEMP].c_str());
	T = maxTemp;
	displayTraversalBool = atoi(configMapper[DISPLAYTRAVERSALBOOL].c_str());
	
	//change to drive simulation
	carStepSize = atoi(configMapper[CARSTEPSIZE].c_str());
	simulation.carStepSize = carStepSize;
	grid = new QStateDrive***[MY_CAR_STATES];
	
	for(int i=0; i<MY_CAR_STATES;i++)
	{
		grid[i] = new QStateDrive**[OTHER_CAR_STATES];
	
		for(int j=0;j<OTHER_CAR_STATES;j++)
		{
			grid[i][j] = new QStateDrive*[DRIVE_LENGTH/carStepSize];
			
			for(int k=0;k<int(DRIVE_LENGTH/carStepSize);k++)
			{
				grid[i][j][k] = new QStateDrive[NUMBER_OF_CLASSES];
			        
			}
		}
	}
	nosInEpisode = atoi(configMapper[NO_STEPS_IN_EPISODE].c_str());
	untrainedEpisodes = atoi(configMapper[UNTRAINED_EPIDODES].c_str());
	//nextStateCertainity = new int[numberOfCells];
	
	
	//lr
	lrConfigPath = configMapper[LRCONFIG];
	wholeTrainingFile = configMapper[WHOLETRAININGFILE];
	trainSetFileSubset = configMapper[TRAINSETFILESUBSET];
	initialTrainingDataCount = atoi(configMapper[INITIALTRAININGDATACOUNT].c_str());


	//SIMULATION
	choiceValEgg= atoi(configMapper[CHOICEVALUE_EGG].c_str());
	choiceValOtherCar = atoi(configMapper[CHOICEVALUE_OTHERCAR].c_str());
	choiceValRest = atoi(configMapper[CHOICEVALUE_REST].c_str());
	simulation.frameRate = atoi(configMapper[WINDOWFRAMERATE].c_str());
	simulation.imagesFolder = configMapper[IMAGES_FOLDER];
	simulation.carDensity = atof(configMapper[CAR_DENSITY].c_str());

	//read QTable from file
	
	if(atoi(configMapper[READQTABLE].c_str())==1)
	{
		readQTableFromFile(configMapper[QTABLEFILE]);
		
		//check to see if correctly read
		cout<<grid[4][2][39][2].qValue[5][2];
		//variable temp set to minimum
		T = minTemp;
	}
}


//read qtable from file

void QTableDrive::readQTableFromFile(string qTableFilePath)
{
        

	ifstream qTableFile;
	vector<string> tokenVector;
		
	int myCarXIndex = 0;
	int otherXIndex = 0;
	int otherYIndex = 0;
	int classIndex = 0;
	int certaintyIndex = 0;
        qTableFile.open(qTableFilePath.c_str());
	if(qTableFile.is_open())
	{
		string line="";
		while(getline(qTableFile,line))
		{
			line = Utility::trim(line);
			Utility::tokenize(line," ",tokenVector);
			if(tokenVector.size()!=NUMBER_OF_ACTIONS)
			{
				myLog<<"InValid config file<<endl";
				throw "Invalid config file";
			}
			
			for(int i=0;i<NUMBER_OF_ACTIONS;i++)
			{
				grid[myCarXIndex][otherXIndex][otherYIndex][classIndex].qValue[certaintyIndex][i] = atof(tokenVector[i].c_str());
			}	
			
			

			
			
			certaintyIndex++;
			if(certaintyIndex==10)
			{
				certaintyIndex = 0;
				
				classIndex++;
				if(classIndex==NUMBER_OF_CLASSES)
				{
					classIndex = 0;

					otherYIndex++;
					if(otherYIndex==(DRIVE_LENGTH/carStepSize))
					{
						otherYIndex = 0;
						otherXIndex++;
						
						if(otherXIndex==OTHER_CAR_STATES)
						{
							otherXIndex=0;
							myCarXIndex++;
							if(myCarXIndex==MY_CAR_STATES)
							{
								myCarXIndex=0;
							}
	
						}
					}
					
				}
			
			}


			tokenVector.clear();
		}
		
	}
        qTableFile.close();
	
	
}



// read in a problem (in libsvm format)
void QTableDrive::readWholeProblem()
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
		if(wholeProblem.y[i]==choiceValEgg)
		{
		        eggIndexes.push_back(i);
		}
		if(wholeProblem.y[i]==choiceValOtherCar)
		{
			otherCarIndexes.push_back(i);
		}
		if(wholeProblem.y[i]==choiceValRest)
		{
			restIndexes.push_back(i);
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


/*
Create partial training set
y have to be changed according to 

#define CHOICEVALUE_EGG "choicevalueegg"
#define CHOICEVALUE_OTHERCAR "choicevalueothercar"
#define CHOICEVALUE_REST "choicevaluerest"


*/


void QTableDrive::createPartialTrainingData()
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
/*
void QTableDrive::createPartialTrainingData()
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

		
		//y have to be changed according to 

		  //#define CHOICEVALUE_EGG "choicevalueegg"
		  //#define CHOICEVALUE_OTHERCAR "choicevalueothercar"
		  //#define CHOICEVALUE_REST "choicevaluerest"

		 //
		if(int(wholeProblem.y[tempIndex]) == choiceValEgg || int(wholeProblem.y[tempIndex]) == choiceValOtherCar)
		{
			trainLRFormat<<wholeProblem.y[tempIndex];
		}
		else
		{
			trainLRFormat<<this->choiceValRest;
		
		}
		
		
		for(int j=0;wholeProblem.x[tempIndex][j+1].index != -1;j++)
		{
			trainLRFormat<<" ";
			trainLRFormat<<wholeProblem.x[tempIndex][j].index<<":"<<wholeProblem.x[tempIndex][j].value;
		}
		trainLRFormat<<endl;
	}
	
}
*/
/*
  binding fixed for qtable
  0  otherCar
  1  eggs
  2  rest
  get the prediction of the current input feature 
  return probability of the max prediction
  
*/

double  QTableDrive::compareClassesAndGetClass(struct feature_node * x,int * classLabel)
{
	//pair<probability,class>
	//sort on probability
	//get first pair 
	vector< pair<double,long double> > probClassVector;
	
	probClassVector.push_back(make_pair(lr.predictProbabiltiyWrapper(x,this->choiceValOtherCar),CLASS_OTHERCAR));
	probClassVector.push_back(make_pair(lr.predictProbabiltiyWrapper(x,this->choiceValEgg),CLASS_EGG));
	probClassVector.push_back(make_pair(lr.predictProbabiltiyWrapper(x,this->choiceValRest),CLASS_REST));

	sort(probClassVector.begin(),probClassVector.end(),Utility::mySort_first);
	
	//logging
	// myLog<<"sorted probClassVector"<<endl;
	// for(int i=0;i<probClassVector.size();i++)
	// {
	//  	myLog<<probClassVector[i].first<<"\t"<<probClassVector[i].second<<endl;
	// }
	// myLog<<endl;
	//logging
	
	*classLabel = probClassVector[0].second;
	return probClassVector[0].first;
}

void QTableDrive::newEpisode()
{


	simulation.initializeWindow();
//	simulation.updateWindow();


	struct feature_node * currImage = NULL;
	
	while (episodeNumber < ceiling && simulation.window.isOpen())
	{
		
		//other Car sprite
		DrawableSprite otherCarDSprite;
		otherCarDSprite.setTexture(simulation.imagesFolder + "/" + simulation.otherCarImage);
		
		
		//egg sprite
		DrawableSprite eggDSprite;
		eggDSprite.setTexture(simulation.imagesFolder + "/" + simulation.eggImage);


		//current state local variables
		int myCarCurrX=-1;
		int otherCurrX=-1;
		int otherCurrY=-1;
		int currClass=-1;
		int currCertainty=-1;
		int currAction =-1;

		int next_myCarCurrX=-1;
		int next_otherCurrX=-1;
		int next_otherCurrY=-1;
		int next_currClass=-1;
		
		//certainty and action start from index = 1
		int next_currCertainty=-1;
		int next_currAction =-1;

		int localSteps = 0;
		int averageReward = 0;
		int carCollisions = 0;
		int eggsCollected = 0;
		int wholeProblemIndex = -1;
		int accuracy =0;
		int reward = -1;
		int numberOfAsks = 0;
		//for each episode
		while(localSteps<nosInEpisode && simulation.window.isOpen())
		{
			//poll for events
			sf::Event event;
			simulation.pollEvent(event);
		
		
			if(simulation.stepWindow)
			{
				simulation.pauseWindow = false;
			}
		
			//wait until paused
			if(simulation.pauseWindow)
			{
				continue;
			}
			
			//if step escape the pause in above wait and again pause for the next iteration
			if(simulation.stepWindow)
			{
				simulation.pauseWindow = true;
				simulation.stepWindow = false;
			}
			
			//poll for events ends
			
                        //display window
			simulation.drawCurrentView();
			

			//curr state from previous iteration/step is next state for current iteration/step
			if(localSteps!=0)
			{
			        myCarCurrX=next_myCarCurrX;
			        otherCurrX=next_otherCurrX;
			        otherCurrY=next_otherCurrY;
			        currClass =next_currClass;
			        currCertainty=next_currCertainty;
				currAction = next_currAction;
			}
			
			
			
			srand(episodeNumber + time(NULL));
		
			if(localSteps==0)
			{
				//a new view
				if(simulation.dSpriteVec.size()==0)
				{
					
					simulation.dSpriteVec.push_back(simulation.getOtherDSpriteWithPD(otherCarDSprite,eggDSprite));
		
				}//end if new sprite
			}
		
			
			if(localSteps!=0)
			{
				if(displayTraversalBool)
				{
					displayTraversal(myCarCurrX,otherCurrX,otherCurrY,currClass,currCertainty,currAction,next_myCarCurrX,next_otherCurrX,next_otherCurrY,next_currClass,next_currCertainty);
					
				}

				
				if(currAction==ACTION_ASK)
				{
					numberOfAsks++;
				}
				//myLog<<"Action : "<<next_currAction<<endl;
				
				//do the action
				//in case of ask next_currClass and next_currcertainty will change
				//action should be currAction
				//doAction(next_currAction,&next_currClass,& next_currCertainty,wholeProblemIndex);
				doAction(currAction,&next_currClass,&next_currCertainty,wholeProblemIndex);
				
                                //simultaneously update the ys'
				// for(int i=0;i<simulation.dSpriteVec.size();i++)
				// {
					
				// 	simulation.dSpriteVec[i].y_pos +=  carStepSize;
				// }
				
				//set mycar x  according to action
				//set other x and y + carStepSize
				//delete out of bound
				//collisions should be recognized in the next step
				simulation.updateCurrentView();
			
				//new dSprite
				if(simulation.dSpriteVec.size()==0)
				{
					
					simulation.dSpriteVec.push_back(simulation.getOtherDSpriteWithPD(otherCarDSprite,eggDSprite));
					
				}//end if new sprite
			
	
				//get reward and reset myCar down the shoulder
				//action should be currAction
                                //int reward = getReward(next_currAction,&carCollisions, &eggsCollected);
				reward = getReward(currAction,&carCollisions, &eggsCollected);
				 
				averageReward += reward; 
				
				
				
				
			}


			//initialize current state local variables
			next_myCarCurrX = mapActualDistToIndex(simulation.myCarDSprite.x_pos,MAP_MYCAR_X);
			next_otherCurrX = mapActualDistToIndex(simulation.dSpriteVec[0].x_pos,MAP_OTHERCAR_X);

			//bug fixed - other was not getting deleted even with y_pos = 400
			next_otherCurrY = mapActualDistToIndex(simulation.dSpriteVec[0].y_pos,MAP_OTHERCAR_Y);
			
			
			
			

                        //crucial - if action was ask - class and certainty are determined in doAction
			if(currAction!=ACTION_ASK)
			{
				//Assign an x depending on
				//1. position of my car x
				//2.position of other car x
				//3.actual what is there infront of my car
				currImage = getImageWithCurrentView(next_myCarCurrX,next_otherCurrX,simulation.dSpriteVec[0].groundTruth,&wholeProblemIndex);
				if(currImage!=NULL)
				{
					
					next_currCertainty = compareClassesAndGetClass(currImage,&next_currClass) * 10 + 1;
					
				}
			}
			
			next_currAction = getAction(next_myCarCurrX,next_otherCurrX,next_otherCurrY,next_currClass,next_currCertainty);
			
			//cannot convert q values for step 0 as next step is not known
			if(localSteps!=0)
			{
				
			
				double tempVal= ((1-learningRate) * grid[myCarCurrX][otherCurrX][otherCurrY][currClass].qValue[currCertainty-1][currAction-1]) + (learningRate* (reward+discountFactor*(grid[next_myCarCurrX][next_otherCurrX][next_otherCurrY][next_currClass].qValue[next_currCertainty-1][next_currAction-1])));
				
				//display state
				std::stringstream ss;
				ss<<"Previous State "<<myCarCurrX<<"\t"<<otherCurrX<<"\t"<<otherCurrY<<"\t"<<currClass<<"\t"<<currCertainty<<"\t"<<currAction<<" QValue: "<<grid[myCarCurrX][otherCurrX][otherCurrY][currClass].qValue[currCertainty-1][currAction-1];
				simulation.infoTextVec.push_back(ss.str());
				ss.str("");
				
				ss<<"Curr State "<<next_myCarCurrX<<"\t"<<next_otherCurrX<<"\t"<<next_otherCurrY<<"\t"<<next_currClass<<"\t"<<next_currCertainty<<"\t"<<next_currAction<<" QValue: "<<grid[next_myCarCurrX][next_otherCurrX][next_otherCurrY][next_currClass].qValue[next_currCertainty-1][next_currAction-1];
				
				simulation.infoTextVec.push_back(ss.str());
				ss.str("");
				
				ss<<"(1-"<<learningRate<<") * "<<grid[myCarCurrX][otherCurrX][otherCurrY][currClass].qValue[currCertainty-1][currAction-1]<<" + "<<learningRate<<"* ("<<reward<<"+"<<discountFactor<<"*("<<grid[next_myCarCurrX][next_otherCurrX][next_otherCurrY][next_currClass].qValue[next_currCertainty-1][next_currAction-1]<<"))";
				
				simulation.infoTextVec.push_back(ss.str());
				ss.str("");
				
			
				ss<<"Previous State QValue after action: "<<tempVal;
				
				simulation.infoTextVec.push_back(ss.str());
				ss.str("");
				
				//display state
				
				grid[myCarCurrX][otherCurrX][otherCurrY][currClass].qValue[currCertainty-1][currAction-1] =tempVal;

			}
				
		

			
			

		
			localSteps++;
		}//end of episode
		


		accuracy = lr.LRTrain(false); 
		

		//logging
		myLog<<episodeNumber<<"\t"<<averageReward<<"\t"<<carCollisions<<"\t"<<eggsCollected<<"\t"<<numberOfAsks<<"\t"<<T<<"\t"<<lr.prob.l<<"\t"<<accuracy<<endl;
	
		//logging
		localSteps = 0;
		
		if(displayTraversalBool)
		{
			displayQTable();
		}
		if(useVariableTempBool && episodeNumber>untrainedEpisodes)
		{
			assignVariableTemp();
		}
	        if(episodeNumber%untrainedEpisodes==0)
		{
			myLog<<endl;
			displayQTable();
			myLog<<endl;
		}
		if(episodeNumber==ceiling-1)
		{
			displayTraversalBool = true;
			myLog<<endl;
			displayQTable();
			myLog<<endl;
			T=minTemp;
			myLog<<endl<<"temp "<<T<<endl;

		}
		
		episodeNumber++;
	}//end of simulation

	if(episodeNumber<ceiling)
	{
		myLog<<endl;
		displayQTable();
		myLog<<endl;
	
	}
}

/*

*/
struct feature_node * QTableDrive::getImageWithCurrentView(int myCarCurrX,int otherCurrX,int groundTruth, int * wholeProblemIndex)
{
	int randomIndex = -1;
	
	//if nothing in the path of myCar
	//add x from rest vector
	//bug - use myCarDSpriteVec.xpos
	if(myCarCurrX !=(otherCurrX+1))
	{
		randomIndex = Utility::getRandom(restIndexes.size());
		*wholeProblemIndex = restIndexes[randomIndex];
		return wholeProblem.x[restIndexes[randomIndex]];
	}
	//there is something in the current path of myCar
	else
	{
		//if there is other car in front of my car
		if(groundTruth==OTHERCARGROUNDTRUTH)
		{
			randomIndex = Utility::getRandom(otherCarIndexes.size());
			*wholeProblemIndex = otherCarIndexes[randomIndex];
			return wholeProblem.x[otherCarIndexes[randomIndex]];

		}
		else if(groundTruth==EGGGROUNDTRUTH)
		{
			randomIndex = Utility::getRandom(eggIndexes.size());
			*wholeProblemIndex = eggIndexes[randomIndex];
			return wholeProblem.x[eggIndexes[randomIndex]];
			
		}
	}

	*wholeProblemIndex= -1;
	return NULL;
}


int QTableDrive::getAction(int myCarX,int otherX, int otherY, int classLabel, int certainty)
{
	
	
	//input variables define the state in QTable
	vector< pair<int,long double> > actionProbVector;
	vector< pair<int,long double> > qValueVector;
	double numerator = 0;
	double denominator=0;
	int tempCertainity =0;
	double tempNumerator = 0;
	//stringstream ss;
	//display state
	std::stringstream ss;
	ss<<"State: "<<myCarX<<"\t"<<otherX<<"\t"<<otherY<<"\t"<<classLabel<<"\t"<<certainty;
	simulation.infoTextVec.push_back(ss.str());
	//display state
	
	//sort existing q -values for given cell and certainity
	for(int i=0;i<NUMBER_OF_ACTIONS;i++)
	{
		tempCertainity = certainty; 
		qValueVector.push_back(make_pair(i,grid[myCarX][otherX][otherY][classLabel].qValue[tempCertainity-1][i]));
	}

	//display Qvalues
	string qValueString("Qvalues: ");

	for(int i=0;i<qValueVector.size();i++)
	{
		ss.str("");
		ss<<qValueVector[i].second;
		qValueString.append(ss.str());
		qValueString.append("\t");
	}
	simulation.infoTextVec.push_back(qValueString);
	//display Qvalues
	
        //sort in descending order
	sort(qValueVector.begin(),qValueVector.end(),Utility::mySort);
	

	/*

	  (e^Q[s,a]/τ)/(∑a eQ[s,a]/τ)
	 */
	for(int i=0;i<qValueVector.size();i++)
	{
		//to avoid out of range e^x values
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

	
	for(int i=0;i<actionProbVector.size();i++)
	{
		actionProbVector[i].second/=denominator;
	}
	
	sort(actionProbVector.begin(),actionProbVector.end(), Utility::mySort);

	//display Qvalues
	string actionValues("ActionValues: ");

	for(int i=0;i<actionProbVector.size();i++)
	{
		ss.str("");
		ss<<actionProbVector[i].first<<":"<<actionProbVector[i].second;
	        actionValues.append(ss.str());
	        actionValues.append("\t");
	}
	simulation.infoTextVec.push_back(actionValues);
	//display Qvalues
	
	//if actions have equal probability and ask (4) is among them return ask
	// double tempQ = actionProbVector[0].second;
	// int index = 1;
	// for(;index<4;index++)
	// {
	// 	if(tempQ==actionProbVector[index].second)
	// 	{
	// 		continue;
	// 	}
	// 	else
	// 		break;
	// }
	
	// if(index>1)
	// {
	// 	for(int i=0;i<index;i++)
	// 	{
	// 		if(actionProbVector[i].first==3)
	// 		{
	// 			return 4;
	// 		}
	// 	}
	// }
	//if actions have equal probability and ask (4) is among them return ask

	//get random number between 0 and 1
	double random = ((double)rand() / RAND_MAX);
	double total=0;
	
	//probabilistically select an action
	for(int i=0;i<actionProbVector.size();i++)
	{
		if(random<=actionProbVector[i].second + total)
		{
			//display Action
			string action("Action: ");
			ss.str("");
			ss<<actionProbVector[i].first+1;
			action.append(ss.str());
			simulation.infoTextVec.push_back(action);
			//display Action
			return actionProbVector[i].first+1;
		}
		else
		{
			total +=  actionProbVector[i].second;
		}

	}

	

}

int QTableDrive::mapActualDistToIndex(int distance,int type)
{
	//myLog<<distance<<endl;
	if(type == MAP_MYCAR_X)
	{
		return (distance -5)/30; 
	}
	else if(type == MAP_OTHERCAR_X)
	{
		return (((distance -5)/30) -1);
	}
	else if(type == MAP_OTHERCAR_Y)
	{
		return distance/(simulation.carStepSize);
	}
	return -1;
}

int QTableDrive::getReward(int action, int * carCollisions, int * eggsCollected)
{

	//if the myCar and other collide
	if(simulation.myCarDSprite.sprite.getGlobalBounds().intersects(simulation.dSpriteVec[0].sprite.getGlobalBounds()))
	{
		

		if(simulation.dSpriteVec[0].groundTruth==OTHERCARGROUNDTRUTH)
		{
			*carCollisions = *carCollisions + 1;
			
			//display Event
			string event("Event: Collision");
			simulation.infoTextVec.push_back(event);
			//display Event
			
			//erase
			simulation.dSpriteVec.erase(simulation.dSpriteVec.begin()+0);
			//erase
			return -100;
		}
		else if(simulation.dSpriteVec[0].groundTruth==EGGGROUNDTRUTH)
		{
			*eggsCollected = *eggsCollected + 1;
			//display Event
			string event("Event: Egg");
			simulation.infoTextVec.push_back(event);
			//display Event
			 
                        //erase
			simulation.dSpriteVec.erase(simulation.dSpriteVec.begin()+0);
			//erase
			return 100;
		}

		
	
	}
	
        //if offtrack
	if(simulation.myCarDSprite.x_pos<0)
	{
		
		//display Event
		string event("Event: Off track");
		simulation.infoTextVec.push_back(event);
		//display Event

		simulation.myCarDSprite.x_pos = 5;
		return -100;
	}
	if(simulation.myCarDSprite.x_pos>150)
	{

		//display Event
		string event("Event: Off track");
		simulation.infoTextVec.push_back(event);
		//display Event

		simulation.myCarDSprite.x_pos = 125;
		return -100;
	}

	//if ask
	if(action == ACTION_ASK)
	{
		//display Event
		string event("Event: Ask");
		simulation.infoTextVec.push_back(event);
		//display Event
			
		return -40;
	}
	//if stay
	//if(action==ACTION_STAY)
	//{
	//	return 0;
	//}
	
	//display Event
	string event("Event: Left/Right/Stay");
	simulation.infoTextVec.push_back(event);
	//display Event
			
	return -10;
}

void QTableDrive::doAction(int action, int * next_currClass,int * next_currCertainty,int wholeProblemIndex)
{
	if(action==ACTION_LEFT)
	{
		simulation.myCarDSprite.x_pos = simulation.myCarDSprite.x_pos -30;  
	}
	else if(action==ACTION_RIGHT)
	{
		simulation.myCarDSprite.x_pos = simulation.myCarDSprite.x_pos +30;  
	}
	else if(action==ACTION_STAY)
	{
		//do nothing
	}
	else if(action==ACTION_ASK)
	{
		//xpos of my car and other car not the same
		if(simulation.myCarDSprite.x_pos != simulation.dSpriteVec[0].x_pos)
		{
			*next_currClass = CLASS_REST;
			*next_currCertainty = 10;
		}
		else if(simulation.dSpriteVec[0].groundTruth==OTHERCARGROUNDTRUTH)
		{
			*next_currClass=CLASS_OTHERCAR;
    			*next_currCertainty = 10;
		}
	       	else if(simulation.dSpriteVec[0].groundTruth==EGGGROUNDTRUTH)
		{
		
			*next_currClass=CLASS_EGG;
			*next_currCertainty = 10;
		}


		

		lr.addInstanceToTraining(wholeProblemIndex,&wholeProblem);

	}
	
}

void QTableDrive::displayQTable()
{
	myLog<<"QTABLE"<<endl;



	for(int i=0; i<MY_CAR_STATES;i++)
	{

	
		for(int j=0;j<OTHER_CAR_STATES;j++)
		{

			
			for(int k=0;k<int(DRIVE_LENGTH/carStepSize);k++)
			{
				
				
				for(int l=0;l<NUMBER_OF_CLASSES;l++)
				{
					myLog<<"cell:"<<i<<" "<<j<<" "<<k<<" "<<l<<endl;
					for(int m=0;m<10;m++)
					{
						for(int n=0;n<NUMBER_OF_ACTIONS;n++)
						{
							myLog<<grid[i][j][k][l].qValue[m][n]<<"     ";
						}
						myLog<<endl;
					}
					
				}
				
		
			}
		}

	}


}

void QTableDrive::assignVariableTemp()
{
	if(T==minTemp)
	{
		return;
	}
	
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


void QTableDrive::displayTraversal(int myCarCurrX,int otherCurrX,int otherCurrY,int currClass,int currCertainty,int currAction,int next_myCarCurrX,int next_otherCurrX,int next_otherCurrY,int next_currClass,int next_currCertainty)
{
	
	string currentState = "";
	stringstream ss;
	ss<<myCarCurrX;
	currentState.append(ss.str());
	ss.str("");
	currentState.append(",");
	ss<<otherCurrX;
	currentState.append(ss.str());
	ss.str("");
	currentState.append(",");
	ss<<otherCurrY;
	
	currentState.append(ss.str());
	ss.str("");
	currentState.append(",");
	ss<<currClass;
	
	currentState.append(ss.str());
	ss.str("");
	
	string nextState = "";
        ss<<next_myCarCurrX;
	
	nextState.append(ss.str());
	ss.str("");
        nextState.append(",");
        ss<<next_otherCurrX;
	
	nextState.append(ss.str());
	ss.str("");
        nextState.append(",");
        ss<<next_otherCurrY;
	
	nextState.append(ss.str());
	ss.str("");
        ss<<next_currClass;
	
	nextState.append(",");
        nextState.append(ss.str());
	ss.str("");
	if(currAction == 1)//left
	{
		myLog<<"Travelling from state ("<<currentState<<" "<<currCertainty<<") taking action left - to state ("<<nextState<<" "<<next_currCertainty<<")"<<endl; 
	}
	else if(currAction==2)//right
	{
	    
		myLog<<"Travelling from state ("<<currentState<<" "<<currCertainty<<") taking action right - to state ("<<nextState<<" "<<next_currCertainty<<")"<<endl; 

	}
	else if(currAction==3)//stay
	{	    
		myLog<<"Travelling from state ("<<currentState<<" "<<currCertainty<<") taking action stay - to state ("<<nextState<<" "<<next_currCertainty<<")"<<endl; 

	}
	else if(currAction==4)//ask
	{
		myLog<<"Travelling from state ("<<currentState<<" "<<currCertainty<<") taking action ask - to state ("<<nextState<<" "<<next_currCertainty<<")"<<endl; 

	}


}
