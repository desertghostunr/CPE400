#include "CentralComputeNode.h"
#include <unordered_set>

#define _INFINITY 9999999

CentralComputeNode::CentralComputeNode()
    : vehicles(), 
    subnetSpeed(), 
    subnetCapacity(), 
    vehiclesAtSubnet(), 
    vechiclesGoingToSubnet(), 
    subnetAdjacencyMatrix(), 
    subnetToIndexTable(),
    jobs()
{

}

CentralComputeNode::~CentralComputeNode() 
{
   
}

void CentralComputeNode::queueJob(Job & job)
{
    jobs.push_back(job);
}

bool CentralComputeNode::computeRoute(Route & route) 
{
    return aStar(route);
}

void CentralComputeNode::directTraffic()
{
    std::list<Job>::iterator jobIter;
    Job job;
    Route route;

    if (jobs.empty()) 
    {
        return;
    }

    //fetch a job
    job = jobs.front();

    //compute a route for the job
    route.start = job.start;
    route.dest = job.dest;

    computeRoute(route);    

    //for each vehicle that can use the route, send it the route

    this->getLock(); // lock this for safe job iteration

    jobIter = jobs.begin;

    while (jobIter != jobs.end)
    {
        job = *jobIter;

        //if the vehicle can use this route
        if(job.start == route.start && job.dest == route.dest)
        {
            if(vehicles[job.id] != NULL)
            {
                vehicles[job.id]->getLock();
                vehicles[job.id]->setRoute(route.route);
                vehicles[job.id]->releaseLock();

                jobIter = jobs.erase(jobIter);

                continue; // if we erased we don't need to increment iter
            }
        }

        ++jobIter;
    }

    this->releaseLock(); // release this


}

void CentralComputeNode::joinNetwork(Vehicle & vehicle)
{
    vehicles[vehicle.getID()] = &vehicle;
    vehiclesAtSubnet[vehicle.getSource()].push_front(vehicle.getID());
}

bool CentralComputeNode::changeRoad(std::string & id, std::string & currentRoad, std::string & newRoad)
{
    if(vehiclesAtSubnet[newRoad].size() < subnetCapacity[newRoad])
    {
        vehiclesAtSubnet[newRoad].push_back(id);
        vehiclesAtSubnet[currentRoad].remove(id);

        return true;
    }

    return false;
}

bool CentralComputeNode::aStar(Route & route)
{
    std::unordered_set<std::string> closedSet, openSet;

    std::map<std::string, std::pair<std::string, long long > > cameFrom;

    std::map<std::string, int> fScore, gScore;

    std::map<std::string, int>::iterator subnetIter;

    std::string current;

    //initialize tables
    for(subnetIter = subnetToIndexTable.begin(); subnetIter != subnetToIndexTable.end(); ++subnetIter)
    {
        fScore[subnetIter->first] = _INFINITY;
        gScore[subnetIter->first] = _INFINITY;
    }

    openSet.emplace(route.start);

    gScore[route.start] = 0;
    fScore[route.start] = vehiclesAtSubnet[route.start].size(); //heuristic

    while(!openSet.empty())
    {
        

    }


    return false;
}

Route CentralComputeNode::reconstructPath
(
    std::map<std::string, std::pair<std::string, long long > > & cameFrom,
    std::pair<std::string, long long > & current,
    std::string start
)
{
    Route route;
    
    route.dest = current.first;

    route.start = start;

    route.route.push_front(current);

    while (current.first != start && current.first.empty())
    {
        current = cameFrom[current.first];
        route.route.push_front(current);
    }

    return route;
}

Job::Job() : start(0), dest(0), id(0)
{

}

Job::~Job() {}

Route::Route() : start(0), dest(0), route()
{

}

Route::~Route() {}
