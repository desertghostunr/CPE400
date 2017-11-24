#include "CentralComputeNode.h"

CentralComputeNode::CentralComputeNode()
    : vehicles(), 
    subnetSpeed(), 
    subnetCapacity(), 
    vehiclesAtSubnet(), 
    vechiclesGoingToSubnet(), 
    subnetAdjacencyMatrix(), 
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

void CentralComputeNode::computeRoute(Route & route) 
{
    // to do compute using AStar
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
                vehicles[job.id]->setRoute(route.route);

                jobIter = jobs.erase(jobIter);

                continue; // if we erased we don't need to increment iter
            }
        }

        ++jobIter;
    }

    this->releaseLock(); // release this


}

void CentralComputeNode::joinNetwork(Vehicle & vehicle, int id)
{
    vehicles[id] = &vehicle;
}

Job::Job() : start(0), dest(0), id(0)
{

}

Job::~Job() {}

Route::Route() : start(0), dest(0), route()
{

}

Route::~Route() {}
