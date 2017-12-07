/**
 * @file Vehicle.cpp
 * 
 * @brief Implementation file for the Vehicle class
 * 
 * @author  Andrew Frost, Richard Millar
 * @version 1.00
 */

// Precompiler Directives =====================================================
#ifndef VEHICLE_CPP
#define VEHICLE_CPP

// Header Files ===============================================================
#include "Vehicle.h"
#include "CentralComputeNode.h"

// Class Implementation =======================================================
/**
 * @brief   Default constructor
 * @details Constructs Vehicle object
 * @note    None
 */
Vehicle::Vehicle() 
    : id(""), sourceAddress(""), destAddress(""), travelTime(), totalTime(), 
    travelTimeLeft(0), route(NULL), routeRequested(false)
{

}

/**
 * @brief       Vehicle Constructor
 * @details     Constructs vehicle object with the specified values
 * 
 * @param[in]   newID       id to assign to object
 * @param[in]   newSource   source of the new object
 * @param[in]   newDest     destination of the new object
 * 
 * @note        None
 */
Vehicle::Vehicle(std::string newID, std::string newSource, std::string newDest)
            : id(newID), sourceAddress(newSource), destAddress(newDest), 
            travelTime(), totalTime(), travelTimeLeft(0), route(NULL), routeRequested(false)
{
    // Constructor Initialized
}


/**
 * @brief       Vehicle Copy Constructor
 * @details     Create a copy of the passed vehicle object
 * 
 * @param[in]   other   Vehicle to make a copy of
 * 
 * @note        None
 */
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


/**
 * @brief   Vehicle destructor
 * @details Destroys vehicle object and data within
 * @note    None
 */
Vehicle::~Vehicle()
{
    // Destroy object
    if(route != NULL)
    {
        delete route;
    }
}


/**
 * @brief   Sets the time the vehicle begins its journey
 * @details Sets the total time to the current time
 * @note    None
 */
void Vehicle::setStartTime() 
{
    totalTime = std::chrono::system_clock::now();
}


/**
 * @brief   Set the depart time of the vehicle
 * @details Sets the travel time to the current time
 * @note    None
 */
void Vehicle::setDepartTime() 
{
    travelTime = std::chrono::system_clock::now();
}


/**
 * @brief   Get the current travel time between nodes
 * @details Returns the difference between now and the travelTime
 * @note    None
 */
std::chrono::duration<double> Vehicle::getTravelTime() const
{
    return (std::chrono::system_clock::now() - travelTime);
}


/**
 * @brief   Get the total travel time
 * @details Returns the totalTime
 * @note    None
 */
std::chrono::duration<double> Vehicle::getTotalTime() const
{
    return (std::chrono::system_clock::now() - totalTime);
}


/**
 * @brief   Get the next vehicle destination
 * @details Returns the next route node if not null
 * @note    None
 */
std::string Vehicle::getNextDestination() const
{
    if(route->empty())
    {
        return std::string("");
    }

    return route->front().first;
}


/**
 * @brief   Shows whether the vehicle has arrived at a node
 * @details Returns whether the travel time is less than the time left
 * @note    None
 */
bool Vehicle::timeRemainingToNextDestination() const
{
    if(std::chrono::duration<double>(travelTimeLeft) > getTravelTime())
    {
        return true;
    }

    return false;
}


/**
 * @brief   Clears the vehicle route
 * @details Deletes the current vehicle route
 * @note    None
 */
void Vehicle::clearRoute()
{
    if (route != NULL)
    {
        delete route;
    }

    route = NULL;
}


/**
 * @brief   Show whether the vehicle has a route
 * @details Returns if route is null
 * @note    None
 */
bool Vehicle::hasRoute() const
{
    return (route != NULL);
}


/**
 * @brief   Get Vehicle ID
 * @details Returns the vehicle ID
 * @note    None
 */
std::string Vehicle::getID()
{
    return id;
}


/**
 * @brief   Get the source
 * @details Returns where the vehicle began from
 * @note    None
 */
std::string Vehicle::getSource()
{
    return sourceAddress;
}


/**
 * @brief   Get the destination
 * @details Returns where the vehicle is going
 * @note    None
 */
std::string Vehicle::getDest()
{
    return destAddress;
}


/**
 * @brief       Determine whether vehicle is traveling to node
 * @details     Returns whether the node is within the vehicle route
 * 
 * @param[in]   node    node to search for in the route
 * 
 * @note        None
 */
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


/**
 * @brief       Request a new route from ccn
 * @details     Send a job request to ccn to set a new route
 * 
 * @param[in]   ccn     Central compute node
 * 
 * @note        None
 */
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

    ccn.queueJob(job);

    routeRequested = true;
}


/**
 * @brief       Sets the vehicle route
 * @details     Sets the route object to the new route
 * 
 * @param[in]   newRoute    new route to set the data to
 * 
 * @note        None
 */
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


/**
 * @brief       Try to do a road change
 * @details     If the object can change its current road do so, else wait
 * 
 * @param[in]   ccn     Central compute node to send requests to
 * @note        None
 */
bool Vehicle::tryRoadChange(CentralComputeNode & ccn)
{
    Job job;
    std::pair<std::string, double> node;
    bool success;


    node = route->front();
    route->pop_front();

    if(!route->empty())
    {
        success = ccn.changeRoad(id, sourceAddress, node.first);
    }
    else 
    {        
        travelTimeLeft = 0;
        return true;
    }
    

    if(!success)
    {
        route->push_front(node);
    }
    else 
    {
        travelTimeLeft = node.second;
        sourceAddress = node.first;        
    }

    return success;
}

#endif