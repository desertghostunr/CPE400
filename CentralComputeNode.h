#ifndef CENTRALCOMPUTENODE_H
#define CENTRALCOMPUTENODE_H

// we need to make this thread safe

//maybe this? https://stackoverflow.com/questions/33895071/how-to-implement-class-lock-objects-for-multithreaded-access

// the second one looks option in the solution might be better

// I wish c++ had something as simple as lock(){} in c# ...

#include <vector>
#include <list>
#include <map>
#include "Vehicle.h"
#include "ThreadSafeObject.h"

struct Job;
struct Route;

class CentralComputeNode : public ThreadSafeObject
{
    
public:
    CentralComputeNode();
    ~CentralComputeNode();

    //the start and finish subnet
    void queueJob(Job & job);

    void computeRoute(Route & route);

    void directTraffic();

    void joinNetwork(Vehicle & vehicle, int id);

private:

    std::map<int, Vehicle*> vehicles; //maps the id of a vehicle to the actual vehicle
    std::map<int, double> subnetSpeed; //in m/s
    std::map<int, int> subnetCapacity; // the number of cars that fit on a subnet
    std::map<int, std::list< int > > vehiclesAtSubnet; //a list of vehicles at each subnet
    std::map<int, std::list< int > > vechiclesGoingToSubnet; //a list of vehicles going to a given subnet

    //this graph has the cost of a subnet in meters between subnets
    std::vector< std::vector< double > > subnetAdjacencyMatrix; //the graph that defines the city

    std::list<Job> jobs; //the jobs that have to be processed

};


struct Job
{
public:
    Job();
    ~Job();
    int start; 
    int dest; 
    int id;
};

struct Route
{
public:
    Route();
    ~Route();

    int start;
    int dest;
    std::list<std::pair<int, long long > > route;
};

#endif