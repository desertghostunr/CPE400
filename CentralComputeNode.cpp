/**
 * @file CentralComputeNode.cpp
 * 
 * @brief Implementation file for the CentralComputeNode class
 * 
 * @author  Andrew Frost, Richard Millar
 * @version 1.00
 */

// Header Files ===============================================================
#include "CentralComputeNode.h"
#include <atomic>

#define _INFINITY 9999999

template<typename Type>
bool GetCheapestNode(std::unordered_set<Type> & set, std::map<Type, double> & fScore, Type & lowest);


/**
 * @brief   Default constructor.
 * @details Constructs an empty CentralComputeNode object
 * @note    None
 */
CentralComputeNode::CentralComputeNode()
    : vehicles(), 
    subnetCapacity(), 
    vehiclesAtSubnet(), 
    subnetAdjacencyMatrix(), 
    subnetToIndexTable(),
    jobs()
{

}

/**
 * @brief   Default destructor.
 * @details Destroys a CentralComputeNode object
 * @note    None
 */
CentralComputeNode::~CentralComputeNode() 
{
   
}


/**
 * @brief       Populates the subnetToIndexTable. 
 * @details     Associates an index value to each subnet ID.
 * 
 * @param[in]   subnets Vector of subnet IDS to assign
 * 
 * @note        None
 */
void CentralComputeNode::buildSubnetToIndexTable(std::vector<std::string> & subnets)
{
    int index;

    for(index = 0; (unsigned)index < subnets.size(); index++)
    {
        subnetToIndexTable[subnets[index]] = index;
    }
}

/**
 * @brief       Returns index of ID
 * @details     Gets and returns the associated index to the ID provided
 * 
 * @param[in]   name    ID to be searched for
 * 
 * @note        None
 */
int CentralComputeNode::getMapIndex(const std::string & name)
{
    if(subnetToIndexTable.count(name) < 1)
    {
        return -1;
    }

    return subnetToIndexTable[name];
}


/**
 * @brief       Assign new map to object
 * @details     Set a new city map within the object
 * @param[in]   map     vector of vector of doubles that detail the distance from 
 *                      each node in the map
 * 
 * @note        None
 */
void CentralComputeNode::setMap(std::vector<std::vector<double> > & map)
{
    subnetAdjacencyMatrix = map;
}


/**
 * @brief       Assign properties to submet
 * @details     Sets the subnet capacity specified by name
 * 
 * @param[in]   name        ID to be searched for
 * @param[in]   capacity    capacity of the subnet to assign
 * 
 * @note        None
 */
void CentralComputeNode::setSubnetProperties(std::string & name, int capacity)
{
    subnetCapacity[name] = capacity;
}


/**
 * @brief       Adds a new job
 * @details     Appends a new job to the end of the queue
 * 
 * @param[in]   job     job to be appended
 * 
 * @note        None
 */
void CentralComputeNode::queueJob(Job & job)
{
    jobs.push_back(job);
}


/**
 * @brief       Computes route
 * @details     Computes the best route from start to end, and returns it
 * 
 * @param[out]  route   route to be computed and returned
 * 
 * @note        None
 */
bool CentralComputeNode::computeRoute(Route & route) 
{
    return aStar(route);
}


/**
 * @brief       Process waiting jobs for routes
 * @details     Processes all pending jobs in the queue, and if there are no vehicles
 *              present on the network, end the simulator.
 * 
 * @param[out]  running    boolean to determine whether the simulator is still
 *                          running.
 * @note        None
 */
void CentralComputeNode::directTraffic(std::atomic_bool &running)
{
    std::list<Job>::iterator jobIter;
    Job job;
    Route route;

    int counter = 0, minCapacity = _INFINITY;
    
    std::list<std::pair<std::string, double> >::iterator pathIter;

    // If there are no more vehicles in the network
    if (vehicles.empty())
    {
        running = false;
        return;
    }
    // If there are no jobs to be run
    if (jobs.empty())
    {
        return;
    }

    //fetch the first job
    job = jobs.front();

    route.start = job.start;
    route.dest = job.dest;

    computeRoute(route);

    if(route.route.empty())
    {
        return;
    }

    //find the minimum capacity
    for(pathIter = route.route.begin(); pathIter != route.route.end(); ++pathIter)
    {
        if(subnetCapacity[pathIter->first] < minCapacity)
        {
            counter = (int)vehiclesAtSubnet[pathIter->first].size();
            minCapacity = subnetCapacity[pathIter->first];
        }
    }

    //for each vehicle that can use the route, send it the route

    jobIter = jobs.begin();

    while (jobIter != jobs.end() && counter <= minCapacity)
    {
        job = *jobIter;

        //if the vehicle can use this route
        if (job.start == route.start && job.dest == route.dest)
        {
            if (vehicles[job.id] != NULL)
            {
                vehicles[job.id]->getLock();
                {
                    vehicles[job.id]->setRoute(route.route);
                    counter++;
                }
                vehicles[job.id]->releaseLock();

                jobIter = jobs.erase(jobIter);

                continue; // if we erased we don't need to increment iter
            }
        }

        ++jobIter;
    }


}


/**
 * @brief       Add vehicle to network
 * @details     Appends vehicle to the vehicle map
 * 
 * @param[in]   vehicle     Vehicle to add to the network
 * 
 * @note        None
 */
void CentralComputeNode::joinNetwork(Vehicle * vehicle)
{
    vehicles[vehicle->getID()] = vehicle;
    vehiclesAtSubnet[vehicle->getSource()].emplace(vehicle->getID());
}


/**
 * @brief       Allow Vehicle to leave network
 * @details     Removes vehicle ID from network
 * 
 * @param[in]   id          ID of vehicle to remove
 * @param[in]   lastNode    last known node vehicle is at
 * 
 * @note        None
 */
void CentralComputeNode::leaveNetwork(const std::string &id, const std::string &lastNode)
{
    vehicles.erase(id);
    vehiclesAtSubnet[lastNode].erase(id);
}


/**
 * @brief       Changes current road of vehicle
 * @details     Determines whether to allow Vehicle to change road, and if so, update
 *              vehicle location and count, else return false
 * 
 * @param[in]   id          vehicle ID
 * @param[in]   currentRoad road vehicle is currently on
 * @param[in]   newRoad     road vehicle is requesting to switch to
 * 
 * @note        None
 */
bool CentralComputeNode::changeRoad(std::string & id, std::string & currentRoad, std::string & newRoad)
{
    if (currentRoad == newRoad)
    {
        return true;
    }
    
    if(vehiclesAtSubnet[newRoad].size() < (unsigned int)subnetCapacity[newRoad])
    {
        vehiclesAtSubnet[newRoad].emplace(id);
        vehiclesAtSubnet[currentRoad].erase(id);

        return true;
    }

    return false;
}


/**
 * @brief       A* Search Algorithm
 * @details     Computes a route based on the starting and end nodes using the A*
 *              Search algorithm
 * 
 * @param[out]  route   route to be computed and returned   
 * 
 * @note        None
 */
bool CentralComputeNode::aStar(Route & route)
{
    std::unordered_set<std::string> closedSet, openSet;

    std::map<std::string, std::string> cameFrom;

    std::map<std::string, double> fScore, gScore;

    std::map<std::string, int>::iterator subnetIter;

    std::string current;

    std::vector<std::string> neighbors;

    int index;

    long long tentativeGScore;
    

    //initialize tables
    for (subnetIter = subnetToIndexTable.begin(); subnetIter != subnetToIndexTable.end(); ++subnetIter)
    {
        fScore[subnetIter->first] = _INFINITY;
        gScore[subnetIter->first] = _INFINITY;
    }

    fScore[route.start] = 0;


    openSet.emplace(route.start);

    gScore[route.start] = 0;    

    while(GetCheapestNode(openSet, fScore, current))
    {

        if(current == route.dest)
        {
            route = reconstructPath(cameFrom, current, route.start);

            return true;
        }

        openSet.erase(current);

        closedSet.emplace(current);

        neighbors = expandNode(current);

        for(index = 0; (unsigned)index < neighbors.size(); index++)
        {
            //if already evaluated
            if(closedSet.count(neighbors[index]) > 0)
            {
                continue;
            }

            //if not in open set
            if(openSet.count(neighbors[index]) < 1)
            {
                openSet.emplace(neighbors[index]);
            }

            tentativeGScore = gScore[current] //get current gScore and add the cost to get to neighbor
                + static_cast<double>((
                    subnetAdjacencyMatrix //get distance to neighbor from current
                    [
                        subnetToIndexTable[current] //translate name to index
                    ]
                    [
                        subnetToIndexTable[neighbors[index]] //translate name to index
                    ])); 

            if(tentativeGScore > gScore[neighbors[index]])
            {
                continue;
            }

            cameFrom[neighbors[index]] = current;

            gScore[neighbors[index]] = tentativeGScore;

            fScore[neighbors[index]] = tentativeGScore 
                + subnetAdjacencyMatrix //get distance to neighbor from current
                [
                    subnetToIndexTable[current] //translate name to index
                ]
                [
                    subnetToIndexTable[neighbors[index]] //translate name to index
                ] 
                * (vehiclesAtSubnet[neighbors[index]].size() + vehiclesAtSubnet[current].size());
        }

    }

    return false;
}


/**
 * @brief       Constructs route between nodes
 * @details     Moves through the cameFrom list and builds a route between current and
 *              Start node
 * 
 * @param[in]   cameFrom  List of nodes and their parents
 * @param[in]   current   Current node
 * @param[in]   start     Start node
 * 
 * @note    None
 */
Route CentralComputeNode::reconstructPath
(
    std::map<std::string, std::string> & cameFrom,
    std::string & current,
    std::string & start
)
{
    Route route;
    
    double cost = 0;

    route.dest = current;

    route.start = start;

    if (cameFrom[current] != start && !cameFrom[current].empty())
    {
        cost = static_cast<double>((
            subnetAdjacencyMatrix //get distance to neighbor from current
            [
                subnetToIndexTable[current] //translate name to index
            ]
            [
                subnetToIndexTable[cameFrom[current]] //translate name to index
            ])); 
    }
    else
    {
        cost = 0;
    }

    route.route.push_front(std::pair<std::string, double>(current, cost));

    while (current != start && !current.empty())
    {
        if (!cameFrom[current].empty())
        {
            cost = static_cast<double>((
                subnetAdjacencyMatrix //get distance to neighbor from current
                [
                    subnetToIndexTable[current] //translate name to index
                ]
                [
                    subnetToIndexTable[cameFrom[current]] //translate name to index
                ])); 
        }
        else
        {
            cost = 0;
        }

        current = cameFrom[current];        


        route.route.push_front(std::pair<std::string, double>(current, cost));
    }

    return route;
}


/**
 * @brief       Expands specified node
 * @details     Looks at all neighbors of the specified node
 * 
 * @param[in]   current current node to look at the neighbors for
 * 
 * @note        None
 */
std::vector<std::string> CentralComputeNode::expandNode(std::string current)
{
    std::map<std::string, int>::iterator iter;
    std::vector<std::string> neighbors;

    int currentAddr = subnetToIndexTable[current];

    for (iter = subnetToIndexTable.begin(); iter != subnetToIndexTable.end(); ++iter)
    {      

        
        //same or not a neighbor
        if (subnetAdjacencyMatrix[currentAddr][iter->second] <= 0)
        {
            continue;
        }

        neighbors.push_back(iter->first);
    }

    return neighbors;
}


/**
 * @brief   Default job constructor
 * @details Constructs a job object
 * @note    None
 */
Job::Job() : start(""), dest(""), id("")
{

}

/**
 * @brief   Default job desructor
 * @details Destroys job object
 * @note    None
 */
Job::~Job() {}


/**
 * @brief   Route default constructor
 * @details Initializes route object
 * @note    None
 */
Route::Route() : start(""), dest(""), route()
{

}


/**
 * @brief   Default route destructor
 * @details Destroys route object
 * @note    None
 */
Route::~Route() {}


/**
 * @brief       Finds the cheapest node
 * @details     Scans the opened set to find the least f-score node.
 * 
 * @param[in]   set     set to be scanned for node
 * @param[in]   fScore  map of node IDs and associated f-scores
 * @param[out]  lowest  Cheapest node found
 *
 * @note        None
 */
template<typename Type>
bool GetCheapestNode(std::unordered_set<Type> & set, std::map<Type, double> & fScore, Type & lowest)
{
    int lowestCost = _INFINITY;

    typename std::unordered_set<Type>::iterator iter;

    if(set.empty())
    {
        return false;
    }

    for(iter = set.begin(); iter != set.end(); ++iter)
    {
        if(fScore.count(*iter) == 0)
        {
            fScore[*iter] = _INFINITY;
        }

        if(fScore[*iter] < lowestCost)
        {
            lowestCost = fScore[*iter];

            lowest = *iter;
        }
    }

    return true;

}
