#ifndef ACTIVE_AND_QLEARN_LR_H
#define ACTIVE_AND_QLEARN_LR_H

#include<iostream>
#include<fstream>
#include<string>
#include<map>
#include<stdio.h>
#include<stdint.h>
#include <cstring>
#include "Utility.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "linear.h"
#define Malloc(type,n) (type *)malloc((n)*sizeof(type))
#define INF HUGE_VAL

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


	struct model* model_;
//train
	struct feature_node *x_space;
	struct parameter param;
	struct problem prob;
//test
	struct feature_node *x;

	int flag_cross_validation;
	int nr_fold;
	double bias;

public:
	string lrConfigFile;
	LRWrapper()
	{

	}
	~LRWrapper()
	{
			destroy_param(&param);
			free(prob.y);
			free(prob.x);
			free(x_space);
			free_and_destroy_model(&model_);
			free(x);
	}
	LRWrapper(string & );
	
	void readMNISTData(string & wholeTrainFile,int );
	void readMNISTData(string & wholeTrainFile,int ,int,int);
		
	double LRTrain(bool);
	void LRTest();
	void readLRConfig();
	char ** argvCreator(int * argvCount, const char *);

	void parse_command_line(int argc, char **argv, char *input_file_name, char *model_file_name);
	void read_problem(const char *filename);
        double do_cross_validation();
	void do_predict(FILE *input, FILE *output);
	double predictProbabiltiyWrapper(struct feature_node * x,int choiceValue);
	void addInstanceToTraining(int wholeProblemIndex, struct problem * wholeProblem);

	friend class QTable;
	friend class QTableDrive;

       

};

#endif
