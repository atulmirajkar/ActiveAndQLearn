#include "ActiveAndQLearnDrive.h"
#include "ActiveAndQLearn.h"
#include "RLStructures.h"

ofstream myLog;

int main()
{

	myLog.open("log");
	QTableBase * qTable;
	qTable = new QTableDrive();
	qTable->initiate();
	myLog.close();
	return 0;
}
