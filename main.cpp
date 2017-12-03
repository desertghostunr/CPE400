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

//the simulator program
//this runs the simulation to test our SDN protocol

//reads in the input file
bool FetchInput(std::string & fileName, CentralComputeNode & ccn, std::vector<Vehicle> & cars);

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
bool FetchInput(std::string & fileName, CentralComputeNode & ccn, std::vector<Vehicle> & cars)
{
    std::stringstream strStream;
    std::fstream fStream;

    std::string buffer;
    std::string type;

    std::string id, start, dest;

    std::vector<std::string> roadIDs;
    std::vector<std::vector<double> > map;

    std::map<std::string, std::map<std::string, int> > cityMap;

    int index, row, col;

    double cost, speed;

    int capacity;

    std::cout << "Opening file: " + fileName << "." << std::endl;

    fStream.open(fileName);

    if(!fStream.is_open())
    {
        std::cout << "Failed to open file: " + fileName << "." << std::endl;
        return false;
    }

    std::cout << "Reading file..." << std::endl;

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

            std::cout << "Car found: " << id << "." << std::endl;
        }
        else if(type == "map")
        {

            std::cout << "Fetching map."<< std::endl;

            //handle first row
            if (!std::getline(fStream, buffer))
            {
                std::cout << "Error: Expected map row header. Row header not found." << std::endl;
                return false;
            }
            
            strStream.str(buffer);

            while(strStream >> id)
            {
                roadIDs.push_back(id);
                std::cout << "Road found: " << id << "." << std::endl;
            }

            //build subnetToIndexTable
            ccn.buildSubnetToIndexTable(roadIDs);


            std::cout << "Initializing map." << std::endl;
            //alloc temp map
            map.resize(roadIDs.size());

            for(index = 0; index < map.size(); index++)
            {
                map[index].resize(roadIDs.size());
            }


            //initialize map values
            for(row = 0; row < map.size(); row++)
            {
                for (col = 0; col < map[row].size(); col++) 
                {
                    if( row == col)
                    {
                        map[row][col] = 0;
                    }
                    else
                    {
                        map[row][col] = -1;
                    }
                }
            }

            std::cout << "Reading in map data." << std::endl;

            //subsequent rows
            while(true)
            {                
                if(!std::getline(fStream, buffer))
                {
                    std::cout << "Error: Expected row. Row not found." << std::endl;
                    return false;
                }

                strStream.clear();
                strStream.str(buffer);

                if(!(strStream >> id))
                {
                    std::cout << "Error: expected id. ID not found. Found: " << strStream.str() << std::endl;
                    return false;
                }

                if(id == "end_map")
                {
                    std::cout << "End of Map reached." << std::endl;
                    break;
                }

                std::cout << "Fetching row: " << id << "." << std::endl;

                col = 0;

                row = ccn.getMapIndex(id);

                if(row == -1 || row >= map.size())
                {
                    std::cout << "Error: Invalid row provided: " << id << "." << std::endl;
                    return false;
                }

                while(strStream >> cost)
                {
                    if(col > map[row].size())
                    {
                        std::cout << "Error: Invalid number of columns found in row: " << id << "." << std::endl;
                        return false;
                    }

                    map[row][col] = cost;
                    col++;
                }              

            }

            //copy map to ccn
            ccn.setMap(map);
            
        }
        else if (type == "road")
        {
            strStream >> id;

            strStream >> capacity;

            //strStream >> speed;

            ccn.setSubnetProperties(id, capacity/*, speed*/);
        }
        else
        {
            std::cout << "Error: unexpected type: " << type << "." << std::endl;
            fStream.close();
            return false;
        }        
    }

    std::cout << "Finished processing file. Closing file." << std::endl;

    fStream.close();

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
    std::string requestRouteSource;

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
                        std::cout << "Car " + car.getID() << " is finished!" << std::endl;                        
                    }
                    consoleLock.releaseLock();

                    ccn.getLock();
                    {
                        ccn.leaveNetwork(car.getID());
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
                            if(requestRouteSource != car.getSource() 
                                && car.getSource() != car.getDest())
                            {
                                car.requestRoute(ccn);
                                requestRouteSource = car.getSource();
                            }

                            consoleLock.getLock();
                            {
                                std::cout << "Car " + car.getID() << " has reached " 
                                    << car.getSource() << "." << std::endl;
                            }
                            consoleLock.releaseLock();
                            car.setDepartTime();
                        }
                    }                    
                    ccn.releaseLock();
                }                
            }
            else if(!routeRequested)
            {

                requestRouteSource = car.getSource();

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

