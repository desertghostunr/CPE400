#include "CentralComputeNode.h"
#include <unordered_set>

#define _INFINITY 9999999

template<typename Type>
bool GetCheapestNode(std::unordered_set<Type> & set, std::map<Type, double> & fScore, Type & lowest);

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
    {
        jobIter = jobs.begin;

        while (jobIter != jobs.end)
        {
            job = *jobIter;

            //if the vehicle can use this route
            if (job.start == route.start && job.dest == route.dest)
            {
                if (vehicles[job.id] != NULL)
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

    std::map<std::string, std::string> cameFrom;

    std::map<std::string, double> fScore, gScore;

    std::map<std::string, int>::iterator subnetIter;

    std::string current;

    std::vector<std::string> neighbors;

    int index;

    long long tentativeGScore;

    this->getLock(); // thread safety
    {
        //initialize tables
        for (subnetIter = subnetToIndexTable.begin(); subnetIter != subnetToIndexTable.end(); ++subnetIter)
        {
            fScore[subnetIter->first] = _INFINITY;
            gScore[subnetIter->first] = _INFINITY;
        }

        fScore[route.start] = vehiclesAtSubnet[route.start].size(); //heuristic
    }
    this->releaseLock();


    openSet.emplace(route.start);

    gScore[route.start] = 0;
    

    while(GetCheapestNode(openSet, fScore, current))
    {        
        if(current == route.dest)
        {
            route = reconstructPath(cameFrom, current, route.start);

            return true;
        }

        closedSet.emplace(current);

        neighbors = expandNode(current);

        for(index = 0; index < neighbors.size(); index++)
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

            this->getLock();
            {
                tentativeGScore = gScore[current] //get current gScore and add the cost to get to neighbor
                    + static_cast<long long>((
                        subnetAdjacencyMatrix //get distance to neighbor from current
                        [
                            subnetToIndexTable[current] //translate name to index
                        ]
                        [
                            subnetToIndexTable[neighbors[index]] //translate name to index
                        ]
                    / subnetSpeed[current])); //divide by speed to get cost
            }
            this->releaseLock();

            if(tentativeGScore > gScore[neighbors[index]])
            {
                continue;
            }

            cameFrom[neighbors[index]] = current;

            gScore[neighbors[index]] = tentativeGScore;

            this->getLock();
            {
                fScore[neighbors[index]] = tentativeGScore + vehiclesAtSubnet[current].size();
            }
            this->releaseLock();
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

    this->getLock();
    {
        if (cameFrom[current] != start && !cameFrom[current].empty())
        {
            cost = static_cast<double>((
                subnetAdjacencyMatrix //get distance to neighbor from current
                [
                    subnetToIndexTable[current] //translate name to index
                ]
                [
                subnetToIndexTable[cameFrom[current]] //translate name to index
                ]
                / subnetSpeed[current])); //divide by speed to get cost
        }
        else
        {
            cost = 0;
        }


        route.route.push_front(std::pair<std::string, double>(current, cost));

        while (current != start && !current.empty())
        {
            current = cameFrom[current];

            if (!cameFrom[current].empty())
            {
                cost = static_cast<double>((
                    subnetAdjacencyMatrix //get distance to neighbor from current
                    [
                        subnetToIndexTable[current] //translate name to index
                    ]
                    [
                        subnetToIndexTable[cameFrom[current]] //translate name to index
                    ]
                / subnetSpeed[current])); //divide by speed to get cost
            }
            else
            {
                cost = 0;
            }

            route.route.push_front(std::pair<std::string, double>(current, cost));
        }
    }
    this->releaseLock();

    return route;
}

std::vector<std::string> CentralComputeNode::expandNode(std::string current)
{
    std::map<std::string, int>::iterator iter;
    std::vector<std::string> neighbors;

    this->getLock();
    {

        int currentAddr = subnetToIndexTable[current];

        for (iter = subnetToIndexTable.begin(); iter != subnetToIndexTable.end(); ++iter)
        {
            //same
            if (iter->first == current)
            {
                continue;
            }

            //not a neighbor
            if (subnetAdjacencyMatrix[currentAddr][iter->second] < 0)
            {
                continue;
            }

            neighbors.push_back(iter->first);
        }

    }
    this->releaseLock();

    return neighbors;
}

Job::Job() : start(0), dest(0), id(0)
{

}

Job::~Job() {}

Route::Route() : start(0), dest(0), route()
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
