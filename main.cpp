#include <iostream>
#include <cstdio>
#include <chrono>
#include <thread>
#include <atomic>
#include <functional>
#include <vector>
#include "Vehicle.h"
#include "CentralComputeNode.h"

//the simulator program
//this runs the simulation to test our SDN protocol

// initializes and runs the simulator in separate threads
void RunSimulator(std::vector<std::thread> & simulatorThreads, 
                  CentralComputeNode & ccn, 
                  std::atomic_bool & running); 

// end the simulator by joining all threads
void EndSimulator(std::vector<std::thread> & simulatorThreads, 
                  std::atomic_bool & running);

//function that runs the compute node in another thread
void WaitFor(long long timeMS); 

//function that runs the compute node in another thread
void ComputeNode(CentralComputeNode& ccn, std::atomic_bool & running);

//function that runs a car in another thread
void Car(CentralComputeNode & ccn, std::atomic_bool & running, Vehicle & car, long long timeStep);


int main() //todo: add command line params, take input, and handle errors
{
    CentralComputeNode ccn;
    std::vector<std::thread> simulatorThreads;

    std::atomic_bool running = true;

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

    RunSimulator(simulatorThreads, ccn, std::ref(running));

    std::cout << "Press any key to end the simulator." << std::endl << std::endl;

    c = getchar();

    std::cout << "Terminating the simulator." << std::endl << std::endl;

    //after keyboard input end the simulator
    EndSimulator(simulatorThreads, std::ref(running));
    
    std::cout << "Simulator terminated." << std::endl;


    return 0;
}

void RunSimulator(std::vector<std::thread> & simulatorThreads, CentralComputeNode & ccn, std::atomic_bool & running)
{
    int index;

    //initialize and start the compute node
    simulatorThreads[0] = std::thread(ComputeNode, ccn, std::ref(running));

    //initialize and start the cars
    for (index = 1; index < simulatorThreads.size(); index++)
    {
        simulatorThreads[index] = std::thread(Car, ccn, std::ref(running));
    }
}

void EndSimulator(std::vector<std::thread> & simulatorThreads, std::atomic_bool & running)
{
    int index;

    running = false;

    for (index = 0; index < simulatorThreads.size(); index++) 
    {
        simulatorThreads[index].join();
    }
}

void WaitFor(long long timeMS)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(timeMS));
}


void ComputeNode(CentralComputeNode& ccn, std::atomic_bool & running) //I think we should add a feature that checks on a cars progress every once in a while
{
    while (running) 
    {
        ccn.getLock();
        {
            ccn.directTraffic();
        }
        ccn.releaseLock();
    }
}

void Car(CentralComputeNode & ccn, std::atomic_bool & running, Vehicle & car, long long timeStep) // need to add init param
{
    ccn.getLock();
    {
        ccn.joinNetwork(car);
    }
    ccn.releaseLock();

    while (running) 
    {
        car.getLock();
        {
            if (car.hasRoute())
            {
                
            }
            else
            {
                //request a route
                car.requestRoute(ccn);
            }
        }
        car.releaseLock();

        //might want to use a random time for the sake of collisions
        WaitFor(300);
    }
}

