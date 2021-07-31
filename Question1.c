/*
 -------------------------------------
 File:    Question1.c
 Project: A04
 -------------------------------------
 Author:  Ryan Campbell
 ID:      181881620
 Email:   camp1620@mylaurier.ca
 Version  2021-07-31
 -------------------------------------
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

typedef struct thread //represents a single customer 
{
	int id;
	pthread_t handle;
} Thread;

void displayStatus(int resourceCount,int customerCount);
void displayArray(int *display, int rows, int columns);

void requestResources(int custId, int resourceCount, int customerCount, int *reqRes);
void releaseResources(int custId, int resourceCount, int *relRes);
void runThreads(int customerCount, Thread *threads);
void* runThread(void *t);
int isStateSafe(int resourceCount, int customerCount);

int readFile(char *fileName, Thread **threads, int resourceCount); //function to read the file content and build array of threads


int *available;
int *maximum;
int *allocation;
int *need;

int numOfResources;

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("no input for resource types \n");
		return -1;
	}
	
	//initialize the available array
	numOfResources = argc - 1;
	available = malloc(sizeof(int) * numOfResources);
	for(int x=0;x!=argc-1;x++)
	{
		available[x] = atoi(argv[x + 1]);
	}	
	Thread *threads = NULL;
	//initialize the threads as well as maximum/need/allocation
	int threadCount = readFile("sample4_in.txt", &threads, numOfResources);
	//display initialized information
	printf("Number of Customers: %d\n",threadCount);
	printf("Currently Available Resources: \n");
	for(int x=0;x != numOfResources;x++)
	{
		printf("%d ",available[x]);
	}
	printf("\nMaximum Resources from file: \n");
	displayArray(maximum,threadCount,numOfResources);
	
	
	int run =1;//set to 0 when time to exit program
	//begin reading input
	printf("Enter Command: ");
	char input[512];
	fgets(input,sizeof(input),stdin);
	while(run){
		input[strcspn(input, "\r\n")] = 0;//remove trailing newline character	
		char *token = strtok(input, " ");
		if(strcmp(token,"Exit") == 0)
		{
			printf("program exiting\n");
			run = 0;
			break;
		}else if(strcmp(token,"Status")== 0){
			displayStatus(numOfResources,threadCount);
		}else if(strcmp(token,"Run")== 0){
			runThreads(threadCount, threads);
		}else if(strcmp(token,"RQ")==0){			
			token = strtok(NULL, " ");//get customer ID
			int customerId = atoi(token);
			
			int *enteredIntegers = malloc(sizeof(int) * numOfResources);
			token = strtok(NULL, " ");//loop to get resource request counts	
			int count = 0;
			while(token != NULL){
				enteredIntegers[count++]= atoi(token);
				token = strtok(NULL, " ");//get next number		
			}
			requestResources(customerId, numOfResources, threadCount, enteredIntegers);
		}else if(strcmp(token,"RL")==0){			
			token = strtok(NULL, " ");//get customer ID
			int customerId = atoi(token);
			
			int *enteredIntegers = malloc(sizeof(int) * numOfResources);
			token = strtok(NULL, " ");//loop to get resource release counts	
			int count = 0;
			while(token != NULL){
				enteredIntegers[count++]= atoi(token);
				token = strtok(NULL, " ");//get next number		
			}
			releaseResources(customerId,numOfResources,enteredIntegers);
		}			
		printf("Enter Command: ");
		fgets(input,sizeof(input),stdin);
	}
	
	
	
	
	
	return 0;
}

void runThreads(int customerCount, Thread *threads){
	
	//find safe execution path
	int *safeExecutionPath = malloc(sizeof(int) * customerCount);
	int *finish = malloc(sizeof(int)* customerCount);
	int *work = malloc(sizeof(int)* numOfResources);	
	for(int x=0;x!=numOfResources;x++){
		work[x] = available[x];
	}
	for(int x=0;x!=customerCount;x++){
		finish[x] = 0;
	}
	
	int finishedCustomerCount = 0;
	while(finishedCustomerCount < customerCount){
		for(int x=0;x!=customerCount;x++){
			if(!finish[x])
			{
				int resAvailable = 1;
				for(int y=0;y!=numOfResources;y++){
					if(need[x * numOfResources + y] > work[y]){//need exceeds available
						resAvailable = 0;
					}
				}
				if(resAvailable){//if resources are available, current customer can do his work and free allocated res
					finish[x] = 1;//this customer is finished
					safeExecutionPath[finishedCustomerCount++] = x;//update the safe execution path
					for(int y=0;y!=numOfResources;y++){
						work[y] += allocation[x * numOfResources + y];//free allocated resources in hypothetical manner
					}
				}
			}
		}
	}
	//print the safe sequence
	printf("Safe Sequence is: ");
	for(int x=0;x!=customerCount;x++){
		printf("%d ",safeExecutionPath[x]);
	}
	printf("\n");
	
	//start the threads in the order as described by safeExecutionPath
	
	for(int x =0;x!=customerCount;x++){//TODO do this in order		
		pthread_create(&(threads[safeExecutionPath[x]].handle), NULL,runThread, &threads[safeExecutionPath[x]]);
		sleep(2);
	}
}

void* runThread(void *t)
{
	int threadID = ((Thread*) t)->id;
	printf("Customer/Thread %d\n",(threadID));
	
	printf("Allocated resources: \n");
	for(int x=0;x!=numOfResources;x++)
	{
		printf("%d ",allocation[threadID * numOfResources + x]);
	}
	printf("\nNeeded: \n");
	for(int x=0;x!=numOfResources;x++)
	{
		printf("%d ",need[threadID * numOfResources + x]);
	}
	printf("\nAvailable: \n");
	for(int x=0;x!=numOfResources;x++)
	{
		printf("%d ",available[x]);
	}
	
	printf("\nThread has started\n");
	printf("Thread has finished\n");
	printf("Thread is releasing resources\n");
	for(int x=0;x!=numOfResources;x++)
	{
		available[x] += allocation[threadID * numOfResources + x];
		allocation[threadID * numOfResources + x] = 0;		
		need[threadID * numOfResources + x] = maximum[threadID * numOfResources + x];		
	}
	printf("New Available: ");
	for(int x=0;x!=numOfResources;x++)
	{
		printf("%d ",available[x]);
	}
	printf("\n");

	pthread_exit(0);
}

void requestResources(int custId, int resourceCount, int customerCount, int *reqRes){
	//check if request exceeds the need
	for(int x=0;x!=resourceCount;x++){
		if(reqRes[x]> need[custId * resourceCount + x]){
			printf("Error: this customer should not be requesting so many resources, try again\n");
			return;
		}
	}
	
	//check if request exceeds available
	for(int x=0;x!=resourceCount;x++){
		if(reqRes[x]> available[x]){
			printf("Error: this customer is requesting more resources than are available, try again\n");
			return;
		}
	}
	//allocate
	for(int x=0;x!=resourceCount;x++)
	{
		allocation[custId * resourceCount + x] +=  reqRes[x];
		need[custId * resourceCount + x] -=  reqRes[x];
		available[x] -=  reqRes[x];
	}	
	int safe = isStateSafe(resourceCount,customerCount);
	if(safe)
	{
		printf("State is safe, and request is satisfied\n");
	}else{//state would be unsafe, undo the allocation
		for(int x=0;x!=resourceCount;x++)
		{
			allocation[custId * resourceCount + x] -=  reqRes[x];
			need[custId * resourceCount + x] +=  reqRes[x];
			available[x] +=  reqRes[x];
		}
		printf("Error: this request cannot be fulfilled as it would lead to an unsafe state, try again\n");
	}
}


//returns 1 if safe, 0 if unsafe, also calculates a safe execution pathway
int isStateSafe(int resourceCount, int customerCount)
{
	int safe = 1;//safe by default
	
	int *finish = malloc(sizeof(int)* customerCount);
	int *work = malloc(sizeof(int)* resourceCount);
	
	for(int x=0;x!=resourceCount;x++){
		work[x] = available[x];
	}
	for(int x=0;x!=customerCount;x++){
		finish[x] = 0;
	}
	
	int found = 1;
	while(found){
		found = 0;
		for(int x=0;x!=customerCount;x++){
			if(!finish[x])
			{
				int resAvailable = 1;
				for(int y=0;y!=resourceCount;y++){
					if(need[x * resourceCount + y] > work[y]){//need exceeds available
						resAvailable = 0;
					}
				}
				if(resAvailable){//if resources are available, current customer can do his work and free allocated res
					found = 1;//found a customer that can free resources
					finish[x] = 1;//this customer is finished
					for(int y=0;y!=resourceCount;y++){
						work[y] += allocation[x * resourceCount + y];//free allocated resources
					}
				}
			}
		}
	}
	
	for(int x=0;x!=customerCount;x++){
		if(!finish[x]){
			safe = 0;//any unfinished customer and this state is unsafe
			break;
		}
	}
	return safe;
}


void releaseResources(int custId, int resourceCount, int *relRes){
	for(int x=0;x!=resourceCount;x++)
	{
		allocation[custId * resourceCount + x] -=  relRes[x];
		need[custId * resourceCount + x] +=  relRes[x];
		available[x] +=  relRes[x];
	}	
	printf("The resources have been released successfully\n");
}

void displayStatus(int resourceCount, int customerCount)
{
	printf("Available Resources: \n");
	for(int x=0;x != resourceCount;x++)
	{
		printf("%d ",available[x]);
	}
	printf("\nMaximum Resources: \n");
	displayArray(maximum,customerCount,resourceCount);
	printf("Allocated Resources: \n");
	displayArray(allocation,customerCount,resourceCount);
	printf("Need Resources: \n");
	displayArray(need,customerCount,resourceCount);
}

void displayArray(int *display, int rows, int columns)
{
	for(int y=0;y != rows;y++)
	{
		for(int x=0;x != columns;x++)
		{
			printf("%d ",display[y * columns + x]);
		}
		printf("\n");
	}
}

int readFile(char *fileName, Thread **threads,int resourceCount)
{
    FILE *fp;
    char buff[255];
	
	int indexInMaxVals =0;
	int maxValsSize = 1;
	int *maximumValues; //all the max resource allocations for customers
	maximumValues = malloc(sizeof(int) * 1);
	
    fp = fopen(fileName, "r");
	int customerCount =0;
	while(fgets(buff, 255, (FILE*)fp))
	{
		customerCount++;		
		buff[strcspn(buff, "\r\n")] = 0;//remove trailing newline character	
		
		char *token = strtok(buff, ",");
		while(token != NULL)//get each number for the line
		{
			int num = atoi(token);
			if(indexInMaxVals >= maxValsSize)//space not available, double the size of the array
			{			
				int *newMaxVals = malloc(sizeof(int) * maxValsSize * 2);
				for(int x=0;x!=maxValsSize;x++){
					newMaxVals[x] = maximumValues[x];
				}
				newMaxVals[maxValsSize] = num;
				maxValsSize  = maxValsSize * 2;
				maximumValues = newMaxVals;
			}		
			maximumValues[indexInMaxVals++] = num;//store the number	
			token = strtok(NULL, ",");//get next number				
		}	
	}
	
	
	maximum  = malloc(sizeof(int) * indexInMaxVals);
	allocation  = malloc(sizeof(int) * indexInMaxVals);
	need  = malloc(sizeof(int) * indexInMaxVals);
	for(int x=0;x!=indexInMaxVals;x=x+resourceCount)
	{
		for(int y=0;y!=resourceCount;y++)
		{
			maximum[x + y] = maximumValues[x + y];
			allocation[x + y] = 0;
			need[x + y] = maximum[x + y];
		}
	}
	*threads = (Thread*) malloc(sizeof(Thread) * customerCount);
	for(int x=0;x!=customerCount;x++){
		(*threads)[x].id = x;
	}
    fclose(fp);
	
	return customerCount;
}

