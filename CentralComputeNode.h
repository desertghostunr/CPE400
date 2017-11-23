#ifndef CENTRALCOMPUTENODE_H
#define CENTRALCOMPUTENODE_H

// we need to make this thread safe

//maybe this? https://stackoverflow.com/questions/33895071/how-to-implement-class-lock-objects-for-multithreaded-access

// the second one looks option in the solution might be better

// I wish c++ had something as simple as lock(){} in c# ...

#include <vector>
#include <list>
#include <map>
#include <queue>
#include "Vehicle.h"
#include "ThreadSafeObject.h"

struct Job;

class CentralComputeNode : public ThreadSafeObject
{
    
public:
    CentralComputeNode();
    ~CentralComputeNode();

    //the start and finish subnet
    void queueJob(Job & job);

    bool computeRoute(std::list<int>& route/*out: route*/, int& start/*out: start*/, int& dest/*out: dest*/);

    void directTraffic();

    void joinNetwork(const Vehicle & vehicle, int id);

private:

    std::map<int, Vehicle*> vehicles; //maps the id of a vehicle to the actual vehicle
    std::map<int, double> subnetSpeed; //in m/s
    std::map<int, int> subnetCapacity; // the number of cars that fit on a subnet
    std::map<int, std::list< int > > vehiclesAtSubnet; //a list of vehicles at each subnet
    std::map<int, std::list< int > > vechiclesGoingToSubnet; //a list of vehicles going to a given subnet

    std::vector< std::vector< double > > subnetAdjacencyMatrix; //the map that defines the city

    std::queue<Job> jobs; //the jobs that have to be processed

};


struct Job
{
public:
    int start; 
    int dest; 
    int id;
};

#endif