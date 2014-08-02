#include "LR.h"
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
/////////////////////////////////////////common to test and train/////////////////////////////////



void exit_with_help();
static char* readline(FILE *input);


struct model* model_;
static char *line = NULL;
static int max_line_len;


//////////////////////////////////////////////////////////////////////////////////////////////////
void parse_command_line(int argc, char **argv, char *input_file_name, char *model_file_name);
void read_problem(const char *filename);
void do_cross_validation();
void print_null(const char *s);

struct feature_node *x_space;
struct parameter param;
struct problem prob;
int flag_cross_validation;
int nr_fold;
double bias;

////////////////////////////////////test////////////////////////////////////////////////////////
void do_predict(FILE *input, FILE *output);
int print_null_test(const char *s,...) {return 0;}
struct feature_node *x;
int flag_predict_probability=0;
static int (*info)(const char *fmt,...) = &printf;
int max_nr_attr = 64;

///////////////////////////////////////////////////////////////////////////////////////////////

LRWrapper::LRWrapper(string & lrConfigPath)
{
	lrConfigFile =lrConfigPath;
	modelFile = "";
}

void LRWrapper::readLRConfig()
{
       	vector<string> tokenVector;
        ifstream config;
	map<string,string> configMapper;
	
	config.open(lrConfigFile.c_str());
	if(config.is_open())
	{
		string line="";
		while(getline(config,line))
		{
			line = Utility::trim(line);
			Utility::tokenize(line,":",tokenVector);
			if(tokenVector.size()!=2)
			{

				throw "Invalid config file";
			}
				

			configMapper[tokenVector[0].c_str()]=tokenVector[1];
			tokenVector.clear();
		}
		
	}
        config.close();
	
	if(configMapper.find(TRAIN_LABEL_FILE)==configMapper.end() || \
	   configMapper.find(TRAIN_IMAGE_FILE)==configMapper.end() ||	\
	   configMapper.find(TEST_LABEL_FILE)==configMapper.end() ||	\
	   configMapper.find(TEST_IMAGE_FILE)==configMapper.end() ||	\
	   configMapper.find(TRAINING_SET_FILE)==configMapper.end() ||	\
	   configMapper.find(TEST_SET_FILE)==configMapper.end() ||	\
	   configMapper.find(TRAIN_OPTIONS)==configMapper.end() ||   \
	   configMapper.find(TEST_OPTIONS)==configMapper.end() ||  \
	   configMapper.find(TEST_OUTPUT_FILE)==configMapper.end())
	{

		throw "Invalid LR config file";

	}

	trainLabelFile = configMapper[TRAIN_LABEL_FILE];
	trainImageFile = configMapper[TRAIN_IMAGE_FILE];
	testLabelFile = configMapper[TEST_LABEL_FILE];
        testImageFile = configMapper[TEST_IMAGE_FILE];
	trainingSetFile =configMapper[TRAINING_SET_FILE];
        testSetFile = configMapper[TEST_SET_FILE];
	if( configMapper.find(MODEL_FILE)!=configMapper.end())
	{
		modelFile = configMapper[MODEL_FILE];
	}
	trainOptions = configMapper[TRAIN_OPTIONS];
        testOptions = configMapper[TEST_OPTIONS];
	testOutputFile = configMapper[TEST_OUTPUT_FILE];

}


void LRWrapper::readMNISTData()
{
	ifstream imageFile(trainImageFile.c_str(), ifstream::binary);

	int magicNumberImage = 0;
	imageFile.read((char*)&magicNumberImage, sizeof(magicNumberImage));
	magicNumberImage = Utility::toLittleEndian(magicNumberImage);
	cout<<magicNumberImage<<endl;

	int numberOfImages =0;
	imageFile.read((char *) & numberOfImages, sizeof(numberOfImages));
	numberOfImages = Utility::toLittleEndian(numberOfImages);
	cout<<numberOfImages<<endl;

	int nRows = 0;
	imageFile.read((char *)& nRows,sizeof(nRows));
	nRows = Utility::toLittleEndian(nRows);
	cout<<nRows<<endl;

	int nCols = 0;
	imageFile.read((char *)&nCols,sizeof(nCols));
	nCols = Utility::toLittleEndian(nCols);
	cout<<nCols<<endl;

	int imageSize = nRows * nCols;
		
	//GET magic number and number of images from label file
	ifstream labelFile(trainLabelFile.c_str(), ifstream::binary);
	
	int magicNumberLabel = 0;
	labelFile.read((char*)&magicNumberLabel, sizeof(magicNumberLabel));
	magicNumberLabel = Utility::toLittleEndian(magicNumberLabel);
	cout<<magicNumberLabel<<endl;

	int numberOfLabels =0;
        labelFile.read((char *) & numberOfLabels, sizeof(numberOfLabels));
	numberOfLabels = Utility::toLittleEndian(numberOfLabels);
	cout<<numberOfLabels<<endl;

	if(numberOfImages!=numberOfLabels)
	{
		throw "Image and label files do not match";
	}

	//write to model input file
	ofstream trainLRFormat(trainingSetFile.c_str());
	uint8_t currLabel = 0;
	uint8_t currPixel = 0;
	map<int,double> sparseFeatures;
        for(int i=0;i<numberOfLabels;i++)
	{
		labelFile.read((char *)&currLabel,sizeof(currLabel));
		//currLabel = Utility::toLittleEndian(currLabel);
		for(int j=0;j<imageSize;j++)
		{
			imageFile.read((char*)&currPixel,sizeof(currPixel));
		
			//currPixel = Utility::toLittleEndian(currPixel);
			//add to sparseFeatures
			//features start from 1
			if(currPixel!=0)
			{
			        sparseFeatures[j+1] =(double)currPixel/255;
			}
			currPixel = 0;
		}
		
		//write label and mpa to file
		map<int,double>::iterator mapIter;
	        trainLRFormat<<(int)currLabel;
		for(mapIter = sparseFeatures.begin();mapIter != sparseFeatures.end();mapIter++)
		{
			trainLRFormat<<" ";
		        trainLRFormat<<mapIter->first<<":"<<mapIter->second; 
		}
	        trainLRFormat<<endl;
		//clear map
		sparseFeatures.clear();
		currLabel = 0;
	}
	labelFile.close();
	imageFile.close();
}


void LRWrapper::LRTrain()
{
	char input_file_name[1024];
	char model_file_name[1024];
	const char *error_msg;
	int argC = 0;
	char ** argv = argvCreator(&argC,TRAIN_PHASE);
	cout<<argC<<endl;
	for(int i=1;i<argC;i++)
	{
		cout<<argv[i];
	}
	parse_command_line(argC, argv, input_file_name, model_file_name);
	read_problem(input_file_name);
	error_msg = check_parameter(&prob,&param);

	if(error_msg)
	{
		fprintf(stderr,"ERROR: %s\n",error_msg);
		exit(1);
	}

	if(flag_cross_validation)
	{
		do_cross_validation();
	}
	else
	{
		model_=train(&prob, &param);
		if(save_model(model_file_name, model_))
		{
			fprintf(stderr,"can't save model to file %s\n",model_file_name);
			exit(1);
		}
		free_and_destroy_model(&model_);
	}
	destroy_param(&param);
	free(prob.y);
	free(prob.x);
	free(x_space);
	free(line);

}


char ** LRWrapper::argvCreator(int * argvCount,const char * phase)
{
	vector<string> tokenVector;
	char ** argv = NULL;
	if(strcmp(phase,TRAIN_PHASE)==0)
	{
		Utility::tokenize(trainOptions," ",tokenVector);

		if(!modelFile.empty())
		{
			//program name + trainsetfile + model file
			*argvCount = tokenVector.size() + 3;
		        argv = new char*[*argvCount];

			//train set file
			argv[*argvCount-1] = new char[strlen(modelFile.c_str())+1];
			strcpy(argv[*argvCount-1],modelFile.c_str());
			argv[*argvCount-1][strlen(modelFile.c_str())] = '\0';
			
			//trainsetfile
			argv[*argvCount - 2] = new char[strlen(trainingSetFile.c_str()) +1];
			strcpy(argv[*argvCount - 2],trainingSetFile.c_str());
			argv[*argvCount - 2][strlen(trainingSetFile.c_str())] = '\0';
		


		}
		else
		{
			//program name + trainsetfile
			*argvCount = tokenVector.size() + 2;
			argv = new char*[*argvCount];

			//trainsetfile
			argv[*argvCount - 1] = new char[strlen(trainingSetFile.c_str()) +1];
			strcpy(argv[*argvCount - 1],trainingSetFile.c_str());
			argv[*argvCount - 1][strlen(trainingSetFile.c_str())] = '\0';

		}
	}
        else if(strcmp(phase,TEST_PHASE)==0)
	{
		Utility::tokenize(testOptions," ",tokenVector);


		//PROGRAM name + test file + model file + outputfile
		*argvCount = tokenVector.size() + 4;
	        argv = new char*[*argvCount];

		//outputfile
		argv[*argvCount - 1] = new char[strlen(testOutputFile.c_str()) +1];
		strcpy(argv[*argvCount - 1],testOutputFile.c_str());
		argv[*argvCount - 1][strlen(testOutputFile.c_str())] = '\0';

		//model file
		argv[*argvCount - 2] = new char[strlen(modelFile.c_str()) +1];
		strcpy(argv[*argvCount - 2],modelFile.c_str());
		argv[*argvCount - 2][strlen(modelFile.c_str())] = '\0';

		//test file
		argv[*argvCount - 3] = new char[strlen(testSetFile.c_str()) +1];
		strcpy(argv[*argvCount - 3],testSetFile.c_str());
		argv[*argvCount - 3][strlen(testSetFile.c_str())] = '\0';

	}
	
	int i=0;
	int j=1;
	for(i=0;i<tokenVector.size();i++,j++)
	{
		argv[j] = new char[strlen(tokenVector[i].c_str()) + 1];
		strcpy(argv[j],tokenVector[i].c_str());
		argv[j][strlen(tokenVector[i].c_str())] = '\0';
	}

	// argv[j] = new char[strlen(trainingSetFile.c_str()) +1];
	// strcpy(argv[j],trainingSetFile.c_str());
	// argv[j][strlen(trainingSetFile.c_str())] = '\0';
	// j++;
	// if(!modelFile.empty())
	// {
	// 	argv[j] = new char[strlen(modelFile.c_str())+1];
	// 	strcpy(argv[j],modelFile.c_str());
	// 	argv[j][strlen(modelFile.c_str())] = '\0';

	// }
	
	

	return argv;
}

void LRWrapper::LRTest()
{
	FILE *input, *output;
	int i;
        int argc;
	char ** argv = argvCreator(&argc,TEST_PHASE);
	cout<<argc<<endl;
        // parse options
	for(i=1;i<argc;i++)
	{
	        if(argv[i][0] != '-') break;
		++i;
		switch(argv[i-1][1])
		{
		case 'b':
			flag_predict_probability = atoi(argv[i]);
			break;
		case 'q':
			info = &print_null_test;
			i--;
			break;
		default:
			fprintf(stderr,"unknown option: -%c\n", argv[i-1][1]);
			exit_with_help();
			break;
		}
	}
	if(i>=argc)
		exit_with_help();

	input = fopen(argv[i],"r");
	if(input == NULL)
	{
		fprintf(stderr,"can't open input file %s\n",argv[i]);
		exit(1);
	}

	output = fopen(argv[i+2],"w");
	if(output == NULL)
	{
		fprintf(stderr,"can't open output file %s\n",argv[i+2]);
		exit(1);
	}

	if((model_=load_model(argv[i+1]))==0)
	{
		fprintf(stderr,"can't open model file %s\n",argv[i+1]);
		exit(1);
	}

	x = (struct feature_node *) malloc(max_nr_attr*sizeof(struct feature_node));
	do_predict(input, output);
	free_and_destroy_model(&model_);
	free(line);
	free(x);
	fclose(input);
	fclose(output);

}
///////////////////////////////////////////////////////////////////////////////////////////////////

void print_null(const char *s) {}

void exit_with_help()
{
	printf(
		"Usage: train [options] training_set_file [model_file]\n"
		"options:\n"
		"-s type : set type of solver (default 1)\n"
		"  for multi-class classification\n"
		" 0 -- L2-regularized logistic regression (primal)\n"
		" 1 -- L2-regularized L2-loss support vector classification (dual)\n"
		" 2 -- L2-regularized L2-loss support vector classification (primal)\n"
		" 3 -- L2-regularized L1-loss support vector classification (dual)\n"
		" 4 -- support vector classification by Crammer and Singer\n"
		" 5 -- L1-regularized L2-loss support vector classification\n"
		" 6 -- L1-regularized logistic regression\n"
		" 7 -- L2-regularized logistic regression (dual)\n"
		"  for regression\n"
		"11 -- L2-regularized L2-loss support vector regression (primal)\n"
		"12 -- L2-regularized L2-loss support vector regression (dual)\n"
		"13 -- L2-regularized L1-loss support vector regression (dual)\n"
		"-c cost : set the parameter C (default 1)\n"
		"-p epsilon : set the epsilon in loss function of SVR (default 0.1)\n"
		"-e epsilon : set tolerance of termination criterion\n"
		"-s 0 and 2\n"
		"|f'(w)|_2 <= eps*min(pos,neg)/l*|f'(w0)|_2,\n"
		"where f is the primal function and pos/neg are # of\n"
		"positive/negative data (default 0.01)\n"
		"-s 11\n"
		"|f'(w)|_2 <= eps*|f'(w0)|_2 (default 0.001)\n"
		"-s 1, 3, 4, and 7\n"
		"Dual maximal violation <= eps; similar to libsvm (default 0.1)\n"
		"-s 5 and 6\n"
		"|f'(w)|_1 <= eps*min(pos,neg)/l*|f'(w0)|_1,\n"
		"where f is the primal function (default 0.01)\n"
		"-s 12 and 13\n"
		"|f'(alpha)|_1 <= eps |f'(alpha0)|,\n"
		"where f is the dual function (default 0.1)\n"
		"-B bias : if bias >= 0, instance x becomes [x; bias]; if < 0, no bias term added (default -1)\n"
		"-wi weight: weights adjust the parameter C of different classes (see README for details)\n"
		"-v n: n-fold cross validation mode\n"
		"-q : quiet mode (no outputs)\n"
		"Usage: predict [options] test_file model_file output_file\n"
		"options:\n"
		"-b probability_estimates: whether to output probability estimates, 0 or 1 (default 0); currently for logistic regression only\n"
		"-q : quiet mode (no outputs)\n"
		);
	exit(1);
}

void exit_input_error(int line_num)
{
	fprintf(stderr,"Wrong input format at line %d\n", line_num);
	exit(1);
}


static char* readline(FILE *input)
{
	int len;

	if(fgets(line,max_line_len,input) == NULL)
		return NULL;

	while(strrchr(line,'\n') == NULL)
	{
		max_line_len *= 2;
		line = (char *) realloc(line,max_line_len);
		len = (int) strlen(line);
		if(fgets(line+len,max_line_len-len,input) == NULL)
			break;
	}
	return line;
}


void do_cross_validation()
{
	int i;
	int total_correct = 0;
	double total_error = 0;
	double sumv = 0, sumy = 0, sumvv = 0, sumyy = 0, sumvy = 0;
	double *target = Malloc(double, prob.l);

	cross_validation(&prob,&param,nr_fold,target);
	if(param.solver_type == L2R_L2LOSS_SVR ||
	   param.solver_type == L2R_L1LOSS_SVR_DUAL ||
	   param.solver_type == L2R_L2LOSS_SVR_DUAL)
	{
		for(i=0;i<prob.l;i++)
		{
			double y = prob.y[i];
			double v = target[i];
			total_error += (v-y)*(v-y);
			sumv += v;
			sumy += y;
			sumvv += v*v;
			sumyy += y*y;
			sumvy += v*y;
		}
		printf("Cross Validation Mean squared error = %g\n",total_error/prob.l);
		printf("Cross Validation Squared correlation coefficient = %g\n",
		       ((prob.l*sumvy-sumv*sumy)*(prob.l*sumvy-sumv*sumy))/
		       ((prob.l*sumvv-sumv*sumv)*(prob.l*sumyy-sumy*sumy))
			);
	}
	else
	{
		for(i=0;i<prob.l;i++)
			if(target[i] == prob.y[i])
				++total_correct;
		printf("Cross Validation Accuracy = %g%%\n",100.0*total_correct/prob.l);
	}

	free(target);
}

void parse_command_line(int argc, char **argv, char *input_file_name, char *model_file_name)
{
	int i;
	void (*print_func)(const char*) = NULL;// default printing to stdout

	// default values
	param.solver_type = L2R_L2LOSS_SVC_DUAL;
	param.C = 1;
	param.eps = INF; // see setting below
	param.p = 0.1;
	param.nr_weight = 0;
	param.weight_label = NULL;
	param.weight = NULL;
	flag_cross_validation = 0;
	bias = -1;

	// parse options
	for(i=1;i<argc;i++)
	{
		if(argv[i][0] != '-') break;
		if(++i>=argc)
			exit_with_help();
		switch(argv[i-1][1])
		{
		case 's':
			param.solver_type = atoi(argv[i]);
			break;

		case 'c':
			param.C = atof(argv[i]);
			break;

		case 'p':
			param.p = atof(argv[i]);
			break;

		case 'e':
			param.eps = atof(argv[i]);
			break;

		case 'B':
			bias = atof(argv[i]);
			break;

		case 'w':
			++param.nr_weight;
			param.weight_label = (int *) realloc(param.weight_label,sizeof(int)*param.nr_weight);
			param.weight = (double *) realloc(param.weight,sizeof(double)*param.nr_weight);
			param.weight_label[param.nr_weight-1] = atoi(&argv[i-1][2]);
			param.weight[param.nr_weight-1] = atof(argv[i]);
			break;

		case 'v':
			flag_cross_validation = 1;
			nr_fold = atoi(argv[i]);
			if(nr_fold < 2)
			{
				fprintf(stderr,"n-fold cross validation: n must >= 2\n");
				exit_with_help();
			}
			break;

		case 'q':
			print_func = &print_null;
			i--;
			break;

		default:
			fprintf(stderr,"unknown option: -%c\n", argv[i-1][1]);
			exit_with_help();
			break;
		}
	}

	set_print_string_function(print_func);

	// determine filenames
	if(i>=argc)
		exit_with_help();

	strcpy(input_file_name, argv[i]);

	if(i<argc-1)
		strcpy(model_file_name,argv[i+1]);
	else
	{
		char *p = strrchr(argv[i],'/');
		if(p==NULL)
			p = argv[i];
		else
			++p;
		sprintf(model_file_name,"%s.model",p);
	}

	if(param.eps == INF)
	{
		switch(param.solver_type)
		{
		case L2R_LR:
		case L2R_L2LOSS_SVC:
			param.eps = 0.01;
			break;
		case L2R_L2LOSS_SVR:
			param.eps = 0.001;
			break;
		case L2R_L2LOSS_SVC_DUAL:
		case L2R_L1LOSS_SVC_DUAL:
		case MCSVM_CS:
		case L2R_LR_DUAL:
			param.eps = 0.1;
			break;
		case L1R_L2LOSS_SVC:
		case L1R_LR:
			param.eps = 0.01;
			break;
		case L2R_L1LOSS_SVR_DUAL:
		case L2R_L2LOSS_SVR_DUAL:
			param.eps = 0.1;
			break;
		}
	}
}

// read in a problem (in libsvm format)
void read_problem(const char *filename)
{
	int max_index, inst_max_index, i;
	long int elements, j;
	FILE *fp = fopen(filename,"r");
	char *endptr;
	char *idx, *val, *label;

	if(fp == NULL)
	{
		fprintf(stderr,"can't open input file %s\n",filename);
		exit(1);
	}

	prob.l = 0;
	elements = 0;
	max_line_len = 1024;
	line = Malloc(char,max_line_len);
	while(readline(fp)!=NULL)
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
		prob.l++;
	}
	rewind(fp);

	prob.bias=bias;

	prob.y = Malloc(double,prob.l);
	prob.x = Malloc(struct feature_node *,prob.l);
	x_space = Malloc(struct feature_node,elements+prob.l);

	max_index = 0;
	j=0;
	for(i=0;i<prob.l;i++)
	{
		inst_max_index = 0; // strtol gives 0 if wrong format
		readline(fp);
		prob.x[i] = &x_space[j];
		label = strtok(line," \t\n");
		if(label == NULL) // empty line
			exit_input_error(i+1);

		prob.y[i] = strtod(label,&endptr);
		if(endptr == label || *endptr != '\0')
			exit_input_error(i+1);

		while(1)
		{
			idx = strtok(NULL,":");
			val = strtok(NULL," \t");

			if(val == NULL)
				break;

			errno = 0;
			x_space[j].index = (int) strtol(idx,&endptr,10);
			if(endptr == idx || errno != 0 || *endptr != '\0' || x_space[j].index <= inst_max_index)
				exit_input_error(i+1);
			else
				inst_max_index = x_space[j].index;

			errno = 0;
			x_space[j].value = strtod(val,&endptr);
			if(endptr == val || errno != 0 || (*endptr != '\0' && !isspace(*endptr)))
				exit_input_error(i+1);

			++j;
		}

		if(inst_max_index > max_index)
			max_index = inst_max_index;

		if(prob.bias >= 0)
			x_space[j++].value = prob.bias;

		x_space[j++].index = -1;
	}

	if(prob.bias >= 0)
	{
		prob.n=max_index+1;
		for(i=1;i<prob.l;i++)
			(prob.x[i]-2)->index = prob.n;
		x_space[j-2].index = prob.n;
	}
	else
		prob.n=max_index;

	fclose(fp);
}
////////////////////////////////////////test////////////////////////////////////////////////

void do_predict(FILE *input, FILE *output)
{
	int correct = 0;
	int total = 0;
	double error = 0;
	double sump = 0, sumt = 0, sumpp = 0, sumtt = 0, sumpt = 0;

	int nr_class=get_nr_class(model_);
	double *prob_estimates=NULL;
	int j, n;
	int nr_feature=get_nr_feature(model_);
	if(model_->bias>=0)
		n=nr_feature+1;
	else
		n=nr_feature;

	if(flag_predict_probability)
	{
		int *labels;

		if(!check_probability_model(model_))
		{
			fprintf(stderr, "probability output is only supported for logistic regression\n");
			exit(1);
		}

		labels=(int *) malloc(nr_class*sizeof(int));
		get_labels(model_,labels);
		prob_estimates = (double *) malloc(nr_class*sizeof(double));
		fprintf(output,"labels");
		for(j=0;j<nr_class;j++)
			fprintf(output," %d",labels[j]);
		fprintf(output,"\n");
		free(labels);
	}

	max_line_len = 1024;
	line = (char *)malloc(max_line_len*sizeof(char));
	while(readline(input) != NULL)
	{
		int i = 0;
		double target_label, predict_label;
		char *idx, *val, *label, *endptr;
		int inst_max_index = 0; // strtol gives 0 if wrong format

		label = strtok(line," \t\n");
		if(label == NULL) // empty line
			exit_input_error(total+1);

		target_label = strtod(label,&endptr);
		if(endptr == label || *endptr != '\0')
			exit_input_error(total+1);

		while(1)
		{
			if(i>=max_nr_attr-2)// need one more for index = -1
			{
				max_nr_attr *= 2;
				x = (struct feature_node *) realloc(x,max_nr_attr*sizeof(struct feature_node));
			}

			idx = strtok(NULL,":");
			val = strtok(NULL," \t");

			if(val == NULL)
				break;
			errno = 0;
			x[i].index = (int) strtol(idx,&endptr,10);
			if(endptr == idx || errno != 0 || *endptr != '\0' || x[i].index <= inst_max_index)
				exit_input_error(total+1);
			else
				inst_max_index = x[i].index;

			errno = 0;
			x[i].value = strtod(val,&endptr);
			if(endptr == val || errno != 0 || (*endptr != '\0' && !isspace(*endptr)))
				exit_input_error(total+1);

			// feature indices larger than those in training are not used
			if(x[i].index <= nr_feature)
				++i;
		}

		if(model_->bias>=0)
		{
			x[i].index = n;
			x[i].value = model_->bias;
			i++;
		}
		x[i].index = -1;

		if(flag_predict_probability)
		{
			int j;
			predict_label = predict_probability(model_,x,prob_estimates);
			fprintf(output,"%g",predict_label);
			for(j=0;j<model_->nr_class;j++)
				fprintf(output," %g",prob_estimates[j]);
			fprintf(output,"\n");
		}
		else
		{
			predict_label = predict(model_,x);
			fprintf(output,"%g\n",predict_label);
		}

		if(predict_label == target_label)
			++correct;
		error += (predict_label-target_label)*(predict_label-target_label);
		sump += predict_label;
		sumt += target_label;
		sumpp += predict_label*predict_label;
		sumtt += target_label*target_label;
		sumpt += predict_label*target_label;
		++total;
	}
	if(model_->param.solver_type==L2R_L2LOSS_SVR ||
	   model_->param.solver_type==L2R_L1LOSS_SVR_DUAL ||
	   model_->param.solver_type==L2R_L2LOSS_SVR_DUAL)
	{
		info("Mean squared error = %g (regression)\n",error/total);
		info("Squared correlation coefficient = %g (regression)\n",
		     ((total*sumpt-sump*sumt)*(total*sumpt-sump*sumt))/
		     ((total*sumpp-sump*sump)*(total*sumtt-sumt*sumt))
			);
	}
	else
		info("Accuracy = %g%% (%d/%d)\n",(double) correct/total*100,correct,total);
	if(flag_predict_probability)
		free(prob_estimates);
}
