#include<iostream>
#include<fstream>
#include<string>
#include<map>
#include<stdio.h>
#include<stdint.h>
#include <cstring>
using namespace std;

#define TRAIN_LABEL_FILE "trainLabelFile"
#define  TRAIN_IMAGE_FILE "trainImageFile"
#define  TEST_LABEL_FILE "testLabelFile"
#define TEST_IMAGE_FILE "testImageFile"
#define TRAINING_SET_FILE "trainingSetFile"
#define TEST_SET_FILE "testSetFile"
#define MODEL_FILE "modelFile"
#define TRAIN_OPTIONS "trainOptions"
#define TEST_OPTIONS "testOptions"
#define TRAIN_PHASE "train"
#define TEST_PHASE "test"
#define TEST_OUTPUT_FILE "testOutputFile"
class LRWrapper{
string lrConfigFile;
string trainLabelFile;
string trainImageFile;
string testLabelFile;
string testImageFile;
string trainingSetFile;
string testSetFile;
string modelFile;
string trainOptions;
string testOptions;
string testOutputFile;
public:
	LRWrapper(string & );
	void readMNISTData();
	void LRTrain();
	void LRTest();
	void readLRConfig();
	char ** argvCreator(int * argvCount, const char *);
	       

};

