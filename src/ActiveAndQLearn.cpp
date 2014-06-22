#include "ActiveAndQLearn.h"
#include"Utility.h"

ofstream myLog;

int main()
{
	int cells = 20;
	QTable qTable(cells);
	myLog.open("bin/log");
	qTable.newEpisode();
	myLog.close();
	return 0;
}

void QTable::newEpisode()
{
	int averageReward = 0;
	int tempCurrCertainity = 0;
	int tempNextCertainity = 0;
	bool displayTraversalBool = false;
	//set 10% of cells to have cans at random
	while(true)
	{
		srand(episodeNumber + time(NULL));
		averageReward = 0;
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
				myLog<<grid[i].isCan<<" "<<grid[i].certainity<<endl; 
			}
			myLog<<endl;
		
			//logging
		}
		//restart at cell 0
		int currentCell = 0;
	
		while(true)
		{
		
			int action = getAction(currentCell);
			int reward = getReward(currentCell, action);
			averageReward +=reward;
			int nextState = getNextState(currentCell,action);
			
			if(displayTraversalBool)
		        displayTraversal(currentCell,action,nextState);

			int nextAction = getAction(nextState);
			tempCurrCertainity = grid[currentCell].certainity;
			tempNextCertainity = grid[nextState].certainity;
			//check for out of bounds
			if(currentCell<0 || currentCell>numberOfCells-1 || tempCurrCertainity <1 || tempCurrCertainity>10 ||action<1 || action>4 || nextState<0 || nextState>numberOfCells-1 || tempNextCertainity <1 || tempNextCertainity>10 || nextAction<1 || nextAction>4)
			{
				myLog<<"Out of bounds Array";
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
				return;
			}
			double tempVal= ((1-stepSize) * grid[currentCell].qValue[tempCurrCertainity-1][action-1]) + (stepSize* (reward+learningRate*(grid[nextState].qValue[tempNextCertainity-1][nextAction-1])));
	
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
		myLog<<"Episode Number "<<episodeNumber<<endl;
		myLog<<"Number of steps "<<numberOfSteps<<endl;
		myLog<<"Average Reward "<<averageReward<<endl;
//logging
		numberOfSteps = 0;
		episodeNumber++;
		
		if(episodeNumber==3000)
		{
			displayTraversalBool = true;
			this->T=1;
		}
		if(episodeNumber==3001)
		{
			//logging
			for(int i=0;i<numberOfCells;i++)
			{
				myLog<<grid[i].isCan<<" "<<grid[i].certainity<<endl; 
			}
			myLog<<endl;
			
			//logging
		
			displayQTable();
			break;
		}
	}//repeat for next episode
}

void QTable::displayTraversal(int currentState,int action,int nextState)
{

	if(action == 1)//left
	{
		myLog<<"Travelling from state ("<<currentState<<" "<<grid[currentState].certainity<<") taking action left - to state ("<<nextState<<" "<<grid[nextState].certainity<<")"<<endl; 
	}
	else if(action==2)//right
	{
		myLog<<"Travelling from state ("<<currentState<<" "<<grid[currentState].certainity<<") taking action right - to state ("<<nextState<<" "<<grid[nextState].certainity<<")"<<endl; 

	}
	else if(action==3)//pickup
	{
		myLog<<"Travelling from state ("<<currentState<<" "<<grid[currentState].certainity<<") taking action pickup - to state ("<<nextState<<" "<<grid[nextState].certainity<<")"<<endl; 

	}
	else if(action==4)//ask
	{
		myLog<<"Travelling from state ("<<currentState<<" "<<grid[currentState].certainity<<") taking action ask - to state ("<<nextState<<" "<<grid[nextState].certainity<<")"<<endl; 

	}
}
void QTable::scatterRandomCans()
{
	int tempRandom=0;
	for(int i=0;i<0.1*numberOfCells;i++)
	{
		tempRandom =Utility::getRandom(numberOfCells); 
		
		//logging
		myLog<<"Random "<<tempRandom<<endl;
		//logging
			
		if(!grid[tempRandom].isCan)
		{
			grid[tempRandom].isCan = true;
		}
		else
		{
			i--;
		}
	       
	}

	
}

void QTable::clearGrid()
{
	//reset cans to 0
		
	for(int i=0;i<numberOfCells;i++)
	{
		grid[i].isCan = false;
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

	//if action is pickup or ask
	//update certainity level for that cell and return the same cell number
	else if(action==3 || action==4)
	{
		if(grid[currentCell].isCan)
		{
			grid[currentCell].certainity = 10;
			return currentCell;
		}
		else
		{
			grid[currentCell].certainity = 1;
			return currentCell;
		}
	}
}

int QTable::getReward(int cellNo, int action)
{

	//reward depending on grid
	if(cellNo==0 && action==1)
	{
		return -15;
	}
	if(cellNo==numberOfCells-1 && action==2)
	{
		return 15;
	}

	//reward for going left or right
	if(action==1)//left
	{
		return -2;
	}
	else if(action==2)//right
	{
		return -2;
	}

	

	//reward depending on certainity
	int certainityVal = grid[cellNo].certainity;
	
        //certainity of no can high
	if(certainityVal>=1 && certainityVal<=3)
	{
	        if(action==3)//pickup
		{
			return -7;
		}
		else if(action==4)//ask
		{
			return -5;
		}

	}
	//uncertainity higher
	else if(certainityVal>=4 && certainityVal<=7)
	{
	        if(action==3)//pickup
		{
			return -5;
		}
		else if(action==4)//ask
		{
			return -3;
		}
	}

	//certainity of can being there is high
	else if(certainityVal>=8 && certainityVal<=10)
	{
		if(action==3)
		{
			return -3;
		}
		else if(action==4)
		{
			return -5;
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
	vector< pair<int,double> > qValueVector;
	double numerator = 0;
	double denominator=0;
	int tempCertainity =0;
	double tempNumerator = 0;
	//sort existing q -values for given cell and certainity
	for(int i=0;i<4;i++)
	{
		tempCertainity = grid[cellNo].certainity;
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

	for(int i=0;i<qValueVector.size();i++)
	{ 
		tempNumerator = double(qValueVector[i].second - qValueVector[0].second)/double(this->T);
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
