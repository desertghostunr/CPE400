#ifndef CENTRALCOMPUTENODE_H
#define CENTRALCOMPUTENODE_H


#include <vector>
#include <list>
#include <map>
#include <string>
#include "Vehicle.h"
#include "ThreadSafeObject.h"

struct Job;
struct Route;
class Vehicle;

class CentralComputeNode : public ThreadSafeObject
{
    
public:
    CentralComputeNode();
    ~CentralComputeNode();

    void buildSubnetToIndexTable(std::vector<std::string> & subnets);

    int getMapIndex(std::string & name);

    void setMap(std::vector<std::vector<double> > & map);

    void setSubnetProperties(std::string & name, int capacity/*, double speed*/);
   
    void queueJob(Job & job);

    bool computeRoute(Route & route);

    void directTraffic();

    void joinNetwork(Vehicle* vehicle);
    void leaveNetwork(const std::string &id);

    bool changeRoad(std::string & id, std::string & currentRoad, std::string & newRoad);

private:

    bool aStar(Route & route);

    Route reconstructPath(std::map<std::string, std::string> & cameFrom,
                                       std::string & current, std::string & start);

    std::vector<std::string> expandNode(std::string current);

    std::map<std::string, Vehicle*> vehicles; //maps the id of a vehicle to the actual vehicle
    std::map<std::string, int> subnetCapacity; // the number of cars that fit on a subnet
    std::map<std::string, std::list< std::string > > vehiclesAtSubnet; //a list of vehicles at each subnet

    //this graph has the cost of a subnet in estimated time to travel between subnets
    std::vector< std::vector< double > > subnetAdjacencyMatrix; //the graph that defines the city
    std::map<std::string, int> subnetToIndexTable;

    std::list<Job> jobs; //the jobs that have to be processed

};


struct Job
{
public:
    Job();
    ~Job();
    std::string start;
    std::string dest;
    std::string id;
};

struct Route
{
public:
    Route();
    ~Route();

    std::string start;
    std::string dest;
    std::list<std::pair<std::string, double > > route;
};

#endif