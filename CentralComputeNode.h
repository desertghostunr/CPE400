#ifndef CENTRALCOMPUTENODE_H
#define CENTRALCOMPUTENODE_H

#include <vector>
#include <list>
#include <map>
#include <queue>
#include "Vehicle.h"

struct Job;

class CentralComputeNode 
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
    std::map<int, std::list< int > > vehiclesAtSubnet; //a list of vehicles at each subnet
    std::map<int, std::list< int > > vechiclesGoingToSubnet; //a list of vehicles going to a given subnet

    std::vector< std::vector< int > > subnetAdjacencyMatrix; //the map that defines the city

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