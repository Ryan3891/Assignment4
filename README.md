# Assignment4 Banker's Algorithm
 Author: Ryan Campbell 
 
 Motivation:
 Implement a functional program that uses the Banker's Algorithm to avoid deadlocks.
 
 Installation:
 Download the makefile, Question1.c, and sample4_in into one folder. Use make to create an executable and ./Question1 to run the program.
 sample4_in must contain x rows with y comma separated numbers. x is the number of customers you will have, y is the number of types of resources that the customers will be using. Each number in the file indicates the maximum amount of a resource the corresponding customer can use at once.
 When runnning the program, you must include exactly y command line arguments that are integers. These indicate how many total resources are available for each of the y resources.
 An example of a proper running of the program with a sample4_in file with 4 resource types would be:  ./Question1 5 2 4 8
 The sample4_in that is part of this repository already contains a set of 5 customers with 4 resource types.

Individual Contribution:
All functions were written by Ryan Campbell.

Features:
The program maintains 4 arrays, available, maximum, allocation, need arrays for each resource, and ensures that no customer can ever be allocated so many resources that any deadlock can occur by using the Banker's algorithm.
A user has 5 commands available.
1. RQ to allocate resources to a customer, 
2. RL to deallocate resources from a customer,
3. Status to receive a full display of the contents of the 4 arrays
4. Run to have the customers execute in turn in a safe sequence and in the end deallocate all their resources once they are done with them.
5. Exit to exit the program.
