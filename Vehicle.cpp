#include "Vehicle.h"

Vehicle::Vehicle(std::string newID, std::string newSource, std::string newDest)
            : id(newID), sourceAddress(newSource), destAddress(newDest), 
            travelTime(0), totalTime(0), route(NULL)
{
    // Constructor Initialized
}

Vehicle::~Vehicle()
{
    // Destroy object
    if(route != NULL)
    {
        delete route;
    }
}

int Vehicle::getTravelTime() const
{
    return travelTime;
}


int Vehicle::getTotalTime() const
{
    return totalTime;
}

std::string Vehicle::getNextDestination() const
{
    return route->front().first;
}


bool Vehicle::hasRoute() const
{
    return (route != NULL);
}


bool Vehicle::hasNode(const std::string &node) const
{
    std::list<std::pair<std::string, long long>>::const_iterator cursor = route->begin();
    std::list<std::pair<std::string, long long>>::const_iterator end = route->end();

    while(cursor != end)
    {
        if(cursor->first == node)
        {
            return true;
        }
        cursor++;
    }

    return false;
}

void Vehicle::requestRoute(CentralComputeNode & ccn)
{
    Job job;

    job.start = sourceAddress;

    job.dest = destAddress;

    job.id = id;

    ccn.getLock();

    ccn.queueJob(job);

    ccn.releaseLock();

}

void Vehicle::setRoute(std::list<std::pair<std::string, long long>> newRoute)
{
    if(!newRoute.empty())
    {
        route = new std::list<std::pair<std::string, long long>>(newRoute);
    }
}

bool Vehicle::tryRoadChange(CentralComputeNode & ccn)
{
    std::pair<std::string, long long> node;
    bool success;

    node = route->front();

    route->pop_front();

    ccn.getLock();
    
    success = ccn.changeRoad(id, sourceAddress, route->front().first);

    ccn.releaseLock();

    if(!success)
    {
        route->push_front(node);
    }
    else 
    {
        sourceAddress = route->front().first;
    }

    return success;
    
}

bool Vehicle::onRoute()
{
    //needs to be implemented

    return false;
}

