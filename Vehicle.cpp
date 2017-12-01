#ifndef VEHICLE_CPP
#define VEHICLE_CPP

#include "Vehicle.h"
#include "CentralComputeNode.h"

Vehicle::Vehicle() 
    : id(""), sourceAddress(""), destAddress(""), travelTime(), totalTime(), 
    travelTimeLeft(0), route(NULL), routeRequested(false)
{

}

Vehicle::Vehicle(std::string newID, std::string newSource, std::string newDest)
            : id(newID), sourceAddress(newSource), destAddress(newDest), 
            travelTime(), totalTime(), travelTimeLeft(0), route(NULL), routeRequested(false)
{
    // Constructor Initialized
}

Vehicle::Vehicle(const Vehicle & other)
    : id(other.id), sourceAddress(other.sourceAddress), destAddress(other.destAddress),
    travelTime(other.travelTime), totalTime(other.totalTime),
    travelTimeLeft(other.travelTimeLeft), route(NULL), routeRequested(other.routeRequested)
{
    if (other.route != NULL) 
    {
        route = new std::list<std::pair<std::string, double> >(*other.route);
    }
}

Vehicle::~Vehicle()
{
    // Destroy object
    if(route != NULL)
    {
        delete route;
    }
}

void Vehicle::setStartTime() 
{
    totalTime = std::chrono::system_clock::now();
}

void Vehicle::setDepartTime() 
{
    travelTime = std::chrono::system_clock::now();
}

std::chrono::duration<double> Vehicle::getTravelTime() const
{
    return (std::chrono::system_clock::now() - travelTime);
}


std::chrono::duration<double> Vehicle::getTotalTime() const
{
    return (std::chrono::system_clock::now() - totalTime);
}

std::string Vehicle::getNextDestination() const
{
    if(route->empty())
    {
        return std::string("");
    }

    return route->front().first;
}

bool Vehicle::timeRemainingToNextDestination() const
{
    if(std::chrono::duration<double>(travelTimeLeft) > getTravelTime())
    {
        return true;
    }

    return false;
}

bool Vehicle::hasRoute() const
{
    return (route != NULL);
}

std::string Vehicle::getID()
{
    return id;
}

std::string Vehicle::getSource()
{
    return sourceAddress;
}

std::string Vehicle::getDest()
{
    return destAddress;
}



bool Vehicle::hasNode(const std::string &node) const
{
    std::list<std::pair<std::string, double>>::const_iterator cursor = route->begin();
    
    while(cursor != route->end())
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

    if(routeRequested)
    {
        return;
    }

    job.start = sourceAddress;

    job.dest = destAddress;

    job.id = id;

    ccn.getLock();
    {
        ccn.queueJob(job);
    }
    ccn.releaseLock();

    routeRequested = true;
}

void Vehicle::setRoute(std::list<std::pair<std::string, double>> newRoute)
{
    if(!newRoute.empty())
    {
        if(route != NULL)
        {
            delete route;
        }
        route = new std::list<std::pair<std::string, double>>(newRoute);
    }

    routeRequested = false;
}

bool Vehicle::tryRoadChange(CentralComputeNode & ccn)
{
    std::pair<std::string, double> node;
    bool success;

    node = route->front();
    route->pop_front();

    ccn.getLock();
    {
        success = ccn.changeRoad(id, sourceAddress, route->front().first);
    }
    ccn.releaseLock();

    if(!success)
    {
        route->push_front(node);
    }
    else 
    {
        travelTimeLeft = node.second;
        sourceAddress = route->front().first;
    }
    return success;
}

#endif