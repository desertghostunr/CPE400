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
    // to do compute
}

void CentralComputeNode::directTraffic()
{
    Route route;

    if (jobs.empty()) 
    {
        return;
    }


}

Job::Job() : start(0), dest(0), id(0)
{

}

Job::~Job() {}

Route::Route() : start(0), dest(0), route()
{

}

Route::~Route() {}
