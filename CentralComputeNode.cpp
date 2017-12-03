#include "CentralComputeNode.h"
#include <atomic>

#define _INFINITY 9999999

template<typename Type>
bool GetCheapestNode(std::unordered_set<Type> & set, std::map<Type, double> & fScore, Type & lowest);

CentralComputeNode::CentralComputeNode()
    : vehicles(), 
    subnetCapacity(), 
    vehiclesAtSubnet(), 
    subnetAdjacencyMatrix(), 
    subnetToIndexTable(),
    jobs()
{

}

CentralComputeNode::~CentralComputeNode() 
{
   
}

void CentralComputeNode::buildSubnetToIndexTable(std::vector<std::string> & subnets)
{
    int index;

    for(index = 0; (unsigned)index < subnets.size(); index++)
    {
        subnetToIndexTable[subnets[index]] = index;
    }
}

int CentralComputeNode::getMapIndex(std::string & name)
{
    if(subnetToIndexTable.count(name) < 1)
    {
        return -1;
    }

    return subnetToIndexTable[name];
}

void CentralComputeNode::setMap(std::vector<std::vector<double> > & map)
{
    subnetAdjacencyMatrix = map;
}

void CentralComputeNode::setSubnetProperties(std::string & name, int capacity)
{
    subnetCapacity[name] = capacity;
}

void CentralComputeNode::queueJob(Job & job)
{
    jobs.push_back(job);
}

bool CentralComputeNode::computeRoute(Route & route) 
{
    return aStar(route);
}

void CentralComputeNode::directTraffic(std::atomic_bool &running)
{
    std::list<Job>::iterator jobIter;
    Job job;
    Route route;

    int counter = 0, minCapacity = _INFINITY;
    
    std::list<std::pair<std::string, double> >::iterator pathIter;


    if (vehicles.empty())
    {
        running = false;
        return;
    }

    if (jobs.empty())
    {
        return;
    }

    //fetch a job
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

void CentralComputeNode::joinNetwork(Vehicle * vehicle)
{
    vehicles[vehicle->getID()] = vehicle;
    vehiclesAtSubnet[vehicle->getSource()].emplace(vehicle->getID());
}

void CentralComputeNode::leaveNetwork(const std::string &id, const std::string &lastNode)
{
    vehicles.erase(id);
    vehiclesAtSubnet[lastNode].erase(id);
}

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

Job::Job() : start(""), dest(""), id("")
{

}

Job::~Job() {}

Route::Route() : start(""), dest(""), route()
{

}

Route::~Route() {}

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
