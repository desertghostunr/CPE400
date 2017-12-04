#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <atomic>
#include <functional>
#include <vector>
#include <string>
#include "ThreadSafeObject.h"
#include "Vehicle.h"
#include "Graph.h"
#include "CentralComputeNode.h"

//the simulator program
//this runs the simulation to test our SDN protocol

//reads in the input file
bool FetchInput(std::string & fileName, CentralComputeNode &ccn, std::vector<Vehicle> & cars);

// end the simulator by joining all threads
void EndSimulator(std::vector<std::thread> & simulatorThreads);

//function that runs the compute node in another thread
void WaitFor(long long timeMS); 

//function that runs the compute node in another thread
void ComputeNode(CentralComputeNode& ccn, std::atomic_bool & running, ThreadSafeObject & consoleLock);

//function that runs a car in another thread
void Car(CentralComputeNode & ccn, std::atomic_bool & running, 
         ThreadSafeObject & consoleLock, Vehicle car, long long timeStep);


int main(int argc, char * argv[])
{
    CentralComputeNode ccn;
    std::vector<Vehicle> vehicles;
    std::vector<std::thread> simulatorThreads;
    std::string fileName;

    std::atomic_bool running(true);

    ThreadSafeObject consoleLock;

    long long tStep = 0;
    int index;

    int c;

    //take input
    if(argc < 2)
    {
        std::cout << "Error: no file name specified. Terminating early." << std::endl;
        return -1;
    }

    fileName = argv[1];

    std::cout << "Reading in simulation data." << std::endl;

    if(!FetchInput(fileName, ccn, vehicles))
    {
        std::cout << "Error: invalid file name or contents. Terminating early." << std::endl;
        return -1;
    }

    //start sim in another thread
    simulatorThreads.resize(vehicles.size());
    

    std::cout << "Starting the simulator." << std::endl << std::endl;

    //initialize and start the compute node

    srand((unsigned)time(0));

    //initialize and start the cars
    for (index = 0; index < simulatorThreads.size(); index++)
    {
        tStep = (rand() % 1500) + 250;
        simulatorThreads[index] = std::thread(Car, std::ref(ccn), std::ref(running), 
                                              std::ref(consoleLock), std::ref(vehicles[index]), tStep);
    }

    WaitFor(2000);

    ComputeNode(ccn, std::ref(running), std::ref(consoleLock));

    consoleLock.getLock(); 
    {
        std::cout << "Terminating the simulator." << std::endl << std::endl;
    }
    consoleLock.releaseLock();

    EndSimulator(simulatorThreads);
    
    std::cout << "Simulator terminated." << std::endl;


    return 0;
}

// Functions ==================================================================
bool FetchInput(std::string &fileName, CentralComputeNode &ccn, std::vector<Vehicle> &cars)
{
    std::ifstream inputFile(fileName);
    std::stringstream arguments;

    std::vector<std::string> roadIDs;
    std::vector<std::vector<double> > map;
    std::map<std::string, std::map<std::string, int> > cityMap;

    std::string command, currentKey, value1, value2, value3;
    int intValue, rowIndex, colIndex;

    if(!inputFile.is_open())
    {
        std::cout << "ERROR: File is invalid, Terminating program..." << std::endl;
        return false;
    }

    while(!inputFile.eof())
    {
        inputFile >> command;
        std::getline(inputFile, value1);

        if(command == "car")
        {
            arguments.str(value1);
            arguments >> value1 >> value2 >> value3;
            cars.push_back(Vehicle(value1, value2, value3));
            std::cout << "Car " << value1 << " found." << std::endl;
        }
        else if(command == "intersect")
        {
            std::cout << "Intersection " << value1 << " found." << std::endl;
            currentKey = value1;
            roadIDs.push_back(value1);
            cityMap[value1];
        }
        else if(command == "neighbor")
        {   
            arguments.str(value1);
            arguments >> value1 >> intValue;
            cityMap[currentKey].insert(std::pair<std::string, int>(value1, intValue));
            std::cout << currentKey << " has neighbor " << value1 << "." << std::endl;
        }
        else if(command[0] == '#')
        {
            continue;
        }
        else
        {
            std::cout << "ERROR: Invalid Command " + command << "." << std::endl;
            return false;
        }
    }

    ccn.buildSubnetToIndexTable(roadIDs);
    map.resize(roadIDs.size());
    for(int index = 0 ; index < map.size(); index++)
    {
        map[index].resize(roadIDs.size());
    }

    for(int row = 0; row < map.size(); row++)
    {
        for(int col = 0; col < map[row].size(); col++)
        {
            if(row == col)
            {
                map[row][col] = 0;
            }
            else
            {
                map[row][col] = -1;
            }
        }
    }
    
    std::map<std::string, std::map<std::string, int> >::iterator iter = cityMap.begin();
    std::map<std::string, int>::iterator neighborIter;
    while(iter != cityMap.end())
    {
        rowIndex = ccn.getMapIndex(iter->first);
        neighborIter = iter->second.begin();
        while(neighborIter != iter->second.end())
        {
            colIndex = ccn.getMapIndex(neighborIter->first);
            map[rowIndex][colIndex] = neighborIter->second;
            neighborIter++;
        }
        iter++;
    }

    
    std::cout << "Closing the file..." << std::endl;
    inputFile.close();
    return true;
}

void EndSimulator(std::vector<std::thread> & simulatorThreads)
{
    int index;

    for (index = 0; index < simulatorThreads.size(); index++) 
    {
        simulatorThreads[index].join();
    }
}

void WaitFor(long long timeMS)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(timeMS));
}


void ComputeNode(CentralComputeNode & ccn, std::atomic_bool & running, ThreadSafeObject & consoleLock)
{
    consoleLock.getLock();
    {
        std::cout << "CCN started." << std::endl;
    }
    consoleLock.releaseLock();

    while (running) 
    {
        ccn.getLock();
        {
            ccn.directTraffic(std::ref(running));
        }
        ccn.releaseLock();
    }
}

void Car(CentralComputeNode & ccn, std::atomic_bool & running, ThreadSafeObject & consoleLock, Vehicle car, long long timeStep) 
{

    bool started = false;

    bool routeRequested = false;

    consoleLock.getLock();
    {
        std::cout << "Vehicle " << car.getID() << " started." << std::endl;
    }
    consoleLock.releaseLock();    

    car.getLock();
    {
        car.setStartTime();

        consoleLock.getLock();
        {
            std::cout << "Car " + car.getID() << " is joining the network." << std::endl;
        }
        consoleLock.releaseLock();
    }
    car.releaseLock();

    ccn.getLock();
    {
        ccn.joinNetwork(&car);
    }
    ccn.releaseLock();

    while (running) 
    {
        car.getLock();
        {
            if (car.hasRoute())
            {
                routeRequested = false;

                //start moving to destination
                if(!started)
                {
                    started = true;
                    car.setDepartTime();

                    consoleLock.getLock();
                    {
                        std::cout << "Car " + car.getID() << " is departing for "
                            << car.getDest() << " from " << car.getSource() << "." << std::endl;
                    }
                    consoleLock.releaseLock();
                    
                }

                //if at dest, then complete
                if (car.getNextDestination() == "")
                {
                    consoleLock.getLock();
                    {
                        std::cout << "Car " + car.getID() << " is finished in: " 
                            << std::chrono::duration_cast<std::chrono::seconds>(car.getTotalTime()).count() 
                            << " seconds." << std::endl;
                    }
                    consoleLock.releaseLock();

                    ccn.getLock();
                    {
                        ccn.leaveNetwork(car.getID(), car.getDest());
                    }
                    ccn.releaseLock();

                    car.releaseLock();

                    return;
                }
                
                if(!car.timeRemainingToNextDestination())
                {
                    ccn.getLock();
                    {
                        if (car.tryRoadChange(ccn))
                        {
                            consoleLock.getLock();
                            {
                                std::cout << "Car " + car.getID() << " has reached " 
                                    << car.getSource() << "." << std::endl;
                            }
                            consoleLock.releaseLock();
                            car.setDepartTime();
                        }
                        else
                        {
                            car.requestRoute(ccn);

                            car.clearRoute();
                        }
                    }                    
                    ccn.releaseLock();
                }                
            }
            else if(!routeRequested)
            {
                routeRequested = true;

                //request a route
                consoleLock.getLock();
                {
                    std::cout << "Car " + car.getID() << " is requesting a route from " << car.getSource() << " to " << car.getDest() << "." << std::endl;
                }
                consoleLock.releaseLock();

                ccn.getLock();
                {
                    car.requestRoute(ccn);
                }
                ccn.releaseLock();
            }
        }
        car.releaseLock();

        //might want to use a random time for the sake of collisions
        WaitFor(timeStep);
    }
}

