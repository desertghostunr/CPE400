/**
 * @file    main.cpp
 * 
 * @brief   Main processing file for the Software Defined Network simulator
 * @details Takes in user input file and starts the simulator, processing each
 *          vehicle route until all are finished
 * 
 * @author  Andrew Frost, Richard Millar
 * @version 1.00
 */

// Header Files ===============================================================
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
#include "CentralComputeNode.h"

// Function Prototypes ========================================================
bool FetchInput(std::string & fileName, CentralComputeNode & ccn, std::vector<Vehicle> & cars);
void EndSimulator(std::vector<std::thread> & simulatorThreads);
void WaitFor(long long timeMS); 
void ComputeNode(CentralComputeNode& ccn, std::atomic_bool & running, ThreadSafeObject & consoleLock);
void Car(CentralComputeNode & ccn, std::atomic_bool & running, 
         ThreadSafeObject & consoleLock, Vehicle car, long long timeStep);


// Main Function ==============================================================
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
/**
 * @brief       Process input file
 * @details     Parses out input file and places the data into the compute node
 * 
 * @param[in]   fileName    file to parse
 * @param[in]   ccn         Central node
 * @param[in]   cars        List of vehicles
 */
bool FetchInput(std::string &fileName, CentralComputeNode &ccn, std::vector<Vehicle> &cars)
{
    std::ifstream inputFile(fileName);
    std::stringstream arguments;

    std::vector<std::string> roadIDs;
    std::vector<int> roadProp;
    std::vector<std::vector<double> > map;
    std::map<std::string, std::map<std::string, int> > cityMap;

    std::map<std::string, std::map<std::string, int> >::iterator cityIter;
    std::map<std::string, int>::iterator neighborIter;

    std::string command, currentKey, value1, value2, value3;
    int intValue, rowIndex, colIndex;

    if(!inputFile.is_open())    //----- If the input file was not opened end
    {
        std::cout << "ERROR: File is invalid, Terminating program..." << std::endl;
        return false;
    }

    std::cout << "Reading File..." << std::endl;

    // Begin processing the file
    while(!inputFile.eof())
    {
        inputFile >> command;   // Read the first block of text into command
        std::getline(inputFile, value1);    // read in the rest of the line

        if(command == "car")    //---- If the command is for a car
        {
            arguments.str(value1);
            arguments >> value1 >> value2 >> value3;
            cars.push_back(Vehicle(value1, value2, value3));
            std::cout << "Car " << value1 << " found." << std::endl;
        }
        else if(command == "intersect") //--- If the command is for an intersection
        {
            arguments.str(value1);
            arguments >> currentKey >> intValue;
            roadIDs.push_back(currentKey);
            roadProp.push_back(intValue);
            cityMap[currentKey];
            std::cout << "Intersection " << currentKey << " found." << std::endl;
        }
        else if(command == "neighbor")  //---- If the command is for a neighbor
        {   
            arguments.str(value1);
            arguments >> value1 >> intValue;
            cityMap[currentKey].insert(std::pair<std::string, int>(value1, intValue));
        }
        else if(command[0] == '#')  //---- If the command is a comment
        {
            continue;
        }
        else    //---- If the command is not recognized
        {
            std::cout << "ERROR: Invalid Command " + command << "." << std::endl;
            inputFile.close();
            return false;
        }
    }
    ccn.buildSubnetToIndexTable(roadIDs);
    map.resize(roadIDs.size());

    // Resize the map to the number of subnets
    for(int index = 0 ; index < map.size(); index++)
    {
        map[index].resize(roadIDs.size());
    }

    // initialize the map values
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
    
    // for each subnet found
    for(cityIter = cityMap.begin(); cityIter != cityMap.end(); cityIter++)
    {
        rowIndex = ccn.getMapIndex(cityIter->first);    //---- set the row to the subnet index

        // Iterate through the neighbors
        for(neighborIter = cityIter->second.begin(); neighborIter != cityIter->second.end(); neighborIter++)
        {
            if(neighborIter->first[0] == '[')   //---- If the neighbor is referenced by index
            {
                std::string neighbor = neighborIter->first;
                neighbor = roadIDs[std::stoi(neighbor.substr(1, neighbor.size()-2)) - 1];
                colIndex = ccn.getMapIndex(neighbor);
            }
            else
            {
                colIndex = ccn.getMapIndex(neighborIter->first);
            }
            map[rowIndex][colIndex] = neighborIter->second;
        }
    }

    // Set each subnet capacity
    for(int index = 0; index < roadIDs.size(); index++)
    {
        ccn.setSubnetProperties(roadIDs[index], roadProp[index]);
    }
    ccn.setMap(map);
    
    std::cout << "Closing the file." << std::endl;
    inputFile.close();
    return true;
}

/**
 * @brief       End the simulator
 * @details     Wait for each thread to join
 * @param[in]   simulatorThreads    list of threads
 */
void EndSimulator(std::vector<std::thread> & simulatorThreads)
{
    for (int index = 0; index < simulatorThreads.size(); index++) 
    {
        simulatorThreads[index].join();
    }
}


/**
 * @brief       Wait for a specified time
 * @details     puts the current thread to sleep
 * 
 * @param[in]   timeMS  time period to wait in milliseconds
 */
void WaitFor(long long timeMS)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(timeMS));
}

/**
 * @brief       Begins the compute node processing
 * @details     Runs the compute node and checks for open jobs periodically
 *    
 * @param[in]   ccn         Main compute node of simulator
 * @param[in]   running     flag to show that the simulator is running
 * @param[in]   consoleLock Lock assigned to the console for output
 */
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


/**
 * @brief       Operations done by each Vehicle object
 * @details     This function runs the car and all its operations. 
 *              
 * 
 * @param[in]   ccn         central compute node
 * @param[in]   running     flag to show simulator is running
 * @param[in]   consoleLock lock for the console output
 * @param[in]   car         main thread object
 * @param[in]   timeStep    
 */
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

    // Join central network
    ccn.getLock();
    {
        ccn.joinNetwork(&car);
    }
    ccn.releaseLock();

    // While the simulator is running
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
                        std::cout << "Car " + car.getID() << " has reached "
                            << car.getDest() << "." << std::endl;

                        std::cout << "Car " + car.getID() << " is finished in: " 
                            << std::chrono::duration_cast<std::chrono::seconds>(car.getTotalTime()).count() 
                            << " seconds." << std::endl;
                    }
                    consoleLock.releaseLock();

                    ccn.getLock();
                    {
                        ccn.leaveNetwork(car.getID(), car.getSource());
                    }
                    ccn.releaseLock();

                    car.releaseLock();

                    return;
                }
                
                // If the car is at a node
                if(!car.timeRemainingToNextDestination())
                {
                    ccn.getLock();
                    {
                        if (car.tryRoadChange(ccn)) //--- Try road change
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
                            consoleLock.getLock();
                            {
                                std::cout << "Car " + car.getID() << " has failed to turn on to "
                                    << car.getNextDestination() << "." << std::endl;
                            }
                            consoleLock.releaseLock();

                            car.clearRoute();

                            routeRequested = false;
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

