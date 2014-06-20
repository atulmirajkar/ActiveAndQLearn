#include "ActiveAndQLearn.h"
#include"Utility.h"

ofstream myLog;

int main()
{
	int cells = 5;
	QTable qTable(cells);
	myLog.open("bin/log");
	qTable.newEpisode();
	myLog.close();
	return 0;
}

void QTable::newEpisode()
{
	int tempRandom=0;
	int tempCurrCertainity = 0;
	int tempNextCertainity = 0;
	//set 10% of cells to have cans at random
	while(true)
	{
		srand(time(NULL));
		for(int i=0;i<0.1*numberOfCells;i++)
		{
			tempRandom =Utility::getRandom(numberOfCells); 
			if(!grid[tempRandom].isCan)
			{
				grid[tempRandom].isCan = true;
			}
			else
			{
				i--;
			}
	       
		}


		//set certainity values to cell with a probability distribution
		setLRCertainity();

		//logging
		/*for(int i=0;i<numberOfCells;i++)
		  {
		  myLog<<grid[i].isCan<<" "<<grid[i].certainity<<endl; 
		  }
		  myLog<<endl;
		*/
		//logging

		int currentCell = 0;
	
		while(true)
		{
		
			//logging
			  myLog<<"Next step started"<<endl;
			  //logging
			int action = getAction(currentCell);
		
		
			//logging
			myLog<<"Action Returned "<<action<<endl;
			//logging
			int reward = getReward(currentCell, action);


			//logging
			  myLog<<"reward Returned "<<reward<<endl;
			  //logging
			int nextState = getNextState(currentCell,action);


			// logging
			  myLog<<"next state  Returned "<<nextState<<endl;
			  //logging
			int nextAction = getAction(nextState);

		
			// logging
			  myLog<<"next Action Returned "<<nextAction<<endl;
			  //logging
	
			  tempCurrCertainity = grid[currentCell].certainity;
			  tempNextCertainity = grid[nextState].certainity;
			  double tempVal= ((1-stepSize) * grid[currentCell].qValue[tempCurrCertainity][action-1]) + (stepSize* (reward+learningRate*(grid[nextState].qValue[tempNextCertainity][nextAction-1])));
	
			//logging
			  myLog<<"Q val assigned "<<tempVal<<endl;
			//logging
			grid[currentCell].qValue[tempCurrCertainity][action-1] = tempVal;

			//logging
			  myLog<<"Q val assigned "<<grid[currentCell].qValue[tempCurrCertainity][action-1]<<endl;
			  myLog<<"cell no "<<currentCell<<endl;
			  myLog<<"cell certainity "<<grid[currentCell].certainity<<endl;
//logging
			
		

			currentCell = nextState;
		
			if(currentCell==numberOfCells-1)
			{
				break;
			}
		

			numberOfSteps++;
		}
		myLog<<"Episode Number "<<episodeNumber;
		myLog<<"Number of steps "<<numberOfSteps;
		numberOfSteps = 0;
		episodeNumber++;
		displayQTable();
	}
}


int QTable::getNextState(int currentCell,int action)
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
	else if(action==3 || action==4)//if pickup or ask
	{
		//1st trial select at random to go left or right
		double chance =  ((double)rand() / RAND_MAX);
		if(chance<=0.5)
		{
			if(currentCell!=0)
			{
				return currentCell-1;
			}
			else
			{
				return currentCell;
			}
		}
		else
		{
			return currentCell+1;
		}
	}
	
	
	/*
	else if(action==4)//if ask
	{
		//1st trial select at random to go left or right
		
	}
	*/
	//error state
	else{
		return -1;
	}
}

int QTable::getReward(int cellNo, int action)
{

	//reward depending on grid
	if(cellNo==0 && action==1)
	{
		return -2;
	}
	if(cellNo==numberOfCells-1 && action==2)
	{
		return 2;
	}

	//reward depending on certainity
	int certainityVal = grid[cellNo].certainity;
	
	if(certainityVal>=1 && certainityVal<=3)
	{
		if(action==1)//left
		{
			return 2;
		}
		else if(action==2)//right
		{
			return 2;
		}
		else if(action==3)//pickup
		{
			return -1;
		}
		else if(action==4)//ask
		{
			return -1;
		}

	}
	else if(certainityVal>=4 && certainityVal<=7)
	{
		if(action==1)
		{
			return 1;
		}
		else if(action==2)
		{
			return 1;
		}
		else if(action==3)
		{
			return -1;
		}
		else if(action==4)
		{
			return 1;
		}
	}
	else if(certainityVal>=8 && certainityVal<=10)
	{
		if(action==1)
		{
			return 1;
		}
		else if(action==2)
		{
			return 1;
		}
		else if(action==3)
		{
			return 2;
		}
		else if(action==4)
		{
			return -1;
		}
	}
     
}



int QTable::getAction(int cellNo)
{
	//0 - left
	//1 - right
	//2  - pickup
	//3 - ask
	//vector<pair<action,probability>>
	vector< pair<int,double> > actionProbVector;
	//actionProbVector.clear();
	double numerator = 0;
	double denominator=0;
	int tempCertainity =0;
	/*//logging
       	myLog<<"ExploitRate "<<exploitRate<<endl;
       	myLog<<"Episode Number "<<episodeNumber<<endl;
	myLog<<"Cell No"<<cellNo<<endl;
	myLog<<"Cell Certainity"<<grid[cellNo].certainity;
	for(int i=0;i<4;i++)
	{
		myLog<<"value "<<grid[cellNo].qValue[grid[cellNo].certainity][i]<<endl;
	}
	//logging
	*/
	for(int i=0;i<4;i++)
	{
	  	/*//logging
		myLog<<"Numberator"<<"\t"<<double(exploitRate * episodeNumber * grid[cellNo].qValue[grid[cellNo].certainity][i])<<endl;
		//logging
		*/
		tempCertainity = grid[cellNo].certainity;
		//logging
		myLog<<"cell No "<<"\t"<<cellNo<<endl;
		myLog<<"certaintiy"<<"\t"<<tempCertainity<<endl;
		
		for(int j=0;j<numberOfCells;j++)
		{
			myLog<<"certaintiy"<<"\t"<<grid[j].certainity<<endl;
		}
		//logging
		numerator = exp(double(exploitRate * episodeNumber * grid[cellNo].qValue[tempCertainity][i]));
		actionProbVector.push_back(make_pair(i,numerator));
		denominator += numerator; 
		/*//logging
		myLog<<"Numberator"<<"\t"<<numerator<<endl;
		//logging
		*/
	}
	
	
	for(int i=0;i<4;i++)
	{
		actionProbVector[i].second/=denominator;
		//grid[cellNo].qValue[grid[cellNo].certainity][i] = actionProbVector[i].second; 
	}

	sort(actionProbVector.begin(),actionProbVector.end(), Utility::mySort);
	/*//logging
	myLog<<"sortedVector"<<endl;
	for(int i=0;i<4;i++)
	{
		myLog<<actionProbVector[i].first<<"\t"<<actionProbVector[i].second<<endl;
	}
	myLog<<endl;
	//logging*/
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
	for(int i=0;i<numberOfCells;i++)
	{
		random = ((double)rand() / RAND_MAX);
		/*//logging
		myLog<<random<<endl;
		//logging
		*/
		if(grid[i].isCan)
		{
			total = 0;
			for(int j=9;j>=0;j--)
			{
				if(random<=pdLRisCan[j] + total)
				{
					grid[i].certainity = j+1;
					break;
				}
				else
				{
					total +=  pdLRisCan[j];
				}
				
			}
		}

		else if(!grid[i].isCan)
		{
			total = 0;
			for(int j=0;j<10;j++)
			{
				if(random<=pdLRisnotCan[j]+total)
				{
					grid[i].certainity =j+1;
					break;
				}
				else
				{
					total +=pdLRisnotCan[j];
				}
			}
		}
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
				myLog<<grid[i].qValue[j][k]<<"\t";
			}
			myLog<<endl;
		}
	}
}
