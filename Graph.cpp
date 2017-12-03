#ifndef GRAPH_CPP
#define GRAPH_CPP

#include "Graph.h"

Graph::Graph()
{

}

Graph::~Graph()
{

}


bool Graph::addNode(const std::string &newKey, const std::map<std::string, int> &neighbors)
{
    std::map<std::string, int>::const_iterator neighborsIterator = neighbors.begin();
    data[newKey] = neighbors;

    while(neighborsIterator != neighbors.end())
    {
        data[neighborsIterator->first];
        neighborsIterator++;
    }
}


std::map<std::string, int>& Graph::operator[](const std::string &key)
{
    return data[key];
}

std::map<std::string, int>& Graph::operator[](const int index)
{
    std::map<std::string, std::map<std::string, int> >::iterator indexIterator = data.begin();
    int iterIndex;

    while(iterIndex < index && indexIterator != data.end())
    {
        indexIterator++;
        iterIndex++;
    }
    return indexIterator->second;
}


#endif