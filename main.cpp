#include <iostream>
#include <cstdio>
#include <thread>
#include <vector>
#include "Vehicle.h"
#include "CentralComputeNode.h"

//the simulator program
//this runs the simulation to test our SDN protocol

// initializes and runs the simulator in separate threads
void RunSimulator(std::vector<std::thread> & simulatorThreads, CentralComputeNode & ccn); 

// end the simulator by joining all threads
void EndSimulator(std::vector<std::thread> & simulatorThreads);

//function that runs the compute node in another thread
void WaitFor(double timeMS); 

//function that runs the compute node in another thread
void ComputeNode(CentralComputeNode& ccn); 

//function that runs a car in another thread
void Car(CentralComputeNode & ccn);


int main() //todo: add command line params and take input and handle errors
{
    CentralComputeNode ccn;
    std::vector<std::thread> simulatorThreads;

    bool error = false;

    int c, numberOfCars = 15;

    //take input

    //error handling

    if (error) 
    {
        return -1;
    }

    //start sim in another thread

    simulatorThreads.resize(numberOfCars + 1);

    std::cout << "Starting the simulator." << std::endl << std::endl;

    RunSimulator(simulatorThreads, ccn);

    std::cout << "Press any key to end the simulator." << std::endl << std::endl;

    c = getchar();

    std::cout << "Terminating the simulator." << std::endl << std::endl;

    //after keyboard input end the simulator
    EndSimulator(simulatorThreads);
    
    std::cout << "Simulator terminated." << std::endl;


    return 0;
}

void RunSimulator(std::vector<std::thread> & simulatorThreads, CentralComputeNode & ccn)
{
    int index;
    //todo: finish implementing

    //initialize and start the compute node


    //initialize and start the cars
    for (index = 1; index < simulatorThreads.size(); index++)
    {
        
    }
}

void EndSimulator(std::vector<std::thread> & simulatorThreads)
{
    int index;

    for (index = 0; index < simulatorThreads.size(); index++) 
    {
        simulatorThreads[index].join();
    }
}

void WaitFor(double timeMS)
{
    //to do implement
}