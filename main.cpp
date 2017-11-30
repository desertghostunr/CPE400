#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <atomic>
#include <functional>
#include <vector>
#include <string>
#include "Vehicle.h"
#include "CentralComputeNode.h"

//the simulator program
//this runs the simulation to test our SDN protocol

//reads in the input file
bool FetchInput(std::string & fileName, CentralComputeNode & ccn, std::vector<Vehicle> & cars);

// initializes and runs the simulator in separate threads
void RunSimulator(std::vector<std::thread> & simulatorThreads, 
                    CentralComputeNode & ccn, 
                    std::vector<Vehicle> & cars,
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


int main(int argc, char * argv[])
{
    CentralComputeNode ccn;
    std::vector < Vehicle > vehicles;
    std::vector<std::thread> simulatorThreads;
    std::string fileName;

    std::atomic_bool running = true;

    int c, numberOfCars = 15;

    //take input
    if(argc < 2)
    {
        std::cout << "Error: no file name specified. Terminating early." << std::endl;
        return -1;
    }

    fileName = argv[1];

    if(!FetchInput(fileName, ccn, vehicles))
    {
        std::cout << "Error: invalid file name or contents. Terminating early." << std::endl;
        return -1;
    }

    //start sim in another thread
    vehicles.resize(numberOfCars + 1);
    simulatorThreads.resize(vehicles.size() + 1);

    

    std::cout << "Starting the simulator." << std::endl << std::endl;

    RunSimulator(simulatorThreads, ccn, vehicles, std::ref(running));

    std::cout << "Press any key to end the simulator." << std::endl << std::endl;

    c = getchar();

    std::cout << "Terminating the simulator." << std::endl << std::endl;

    //after keyboard input end the simulator
    EndSimulator(simulatorThreads, std::ref(running));
    
    std::cout << "Simulator terminated." << std::endl;


    return 0;
}

bool FetchInput(std::string & fileName, CentralComputeNode & ccn, std::vector<Vehicle> & cars)
{
    std::stringstream strStream;
    std::fstream fStream;

    std::string buffer;
    std::string type;

    std::string id, start, dest;

    fStream.open(fileName);

    if(!fStream.is_open())
    {
        return false;
    }

    // get line and convert it's contents
    while(std::getline(fStream, buffer))
    {
        strStream.str(buffer); 

        strStream >> type;

        if(type == "car")
        {
            strStream >> id;
            strStream >> start;
            strStream >> dest;

            cars.push_back(Vehicle(id, start, dest));
        }
        else if(type == "map")
        {
            //handle first row

            //TO DO: WRITE CODE FOR THIS

            //subsequent rows
            while(true)
            {
                std::getline(fStream, buffer);

                strStream.str(buffer);

                strStream >> type;

                if(type == "end_map")
                {
                    break;
                }
                
                //TO DO: WRITE LOGIC FOR BUIDLING MAP!
            }
            
        }
        else if (type == "road")
        {
            //TO DO: WRITE LOGIC FOR ACTUALLY READING IN ROAD INFO
        }
        else
        {
            fStream.close();
            return false;
        }        
    }

    fStream.close();

    return true;
}

void RunSimulator
(
    std::vector<std::thread> & simulatorThreads, 
    CentralComputeNode & ccn, 
    std::vector<Vehicle> & cars,
    std::atomic_bool & running
)
{
    long long tStep = 0;
    int index;

    //initialize and start the compute node
    simulatorThreads[0] = std::thread(ComputeNode, ccn, std::ref(running));

    srand((unsigned)time(0));

    //initialize and start the cars
    for (index = 1; index < simulatorThreads.size(); index++)
    {
        tStep = (rand() % 1500) + 100;
        simulatorThreads[index] = std::thread(Car, ccn, std::ref(running), cars[index - 1], tStep);
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

        WaitFor(30);
    }
}

void Car(CentralComputeNode & ccn, std::atomic_bool & running, Vehicle & car, long long timeStep) // need to add init param
{

    bool started = false;

    car.setStartTime();
    
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
                //start moving to destination
                if(!started)
                {
                    started = true;
                    car.setDepartTime();
                    
                }

                //if at dest, then complete
                if (car.getNextDestination() == "" && !car.timeRemainingToNextDestination())
                {
                    break;
                }
                
                if(!car.timeRemainingToNextDestination())
                {
                    if(car.tryRoadChange(ccn))
                    {
                        car.setDepartTime();
                    }
                }                
            }
            else
            {
                //request a route
                car.requestRoute(ccn);
            }
        }
        car.releaseLock();

        //might want to use a random time for the sake of collisions
        WaitFor(timeStep);
    }
}

