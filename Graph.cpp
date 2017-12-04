#ifndef GRAPH_CPP
#define GRAPH_CPP

#include <stdexcept>
#include <cstdlib>
#include "Graph.h"

Graph::Graph()
{

}

Graph::~Graph()
{

}


void Graph::buildNodes(const std::vector<std::string> &nodes, const std::vector<std::pair<std::string, int> > &neighbors)
{
    std::string value;
    int dataIndex;

    for(int index = 0; index < nodes.size(); index++)
    {
        data[nodes[index]];
    }

    for(int index = 0; index < neighbors.size(); index++)
    {
        value = neighbors[index].first;
        if(value[0] == '[')
        {
            dataIndex = std::atoi(value.substr(0, (value.size() - 2)).c_str());
        }
    }
}

std::map<std::string, int>& Graph::operator[](const std::string &key)
{
    return data[key];
}

std::map<std::string, int>& Graph::operator[](const int index) throw()
{
    std::string key = getKey(index);
    return data[key];
}

std::string Graph::getKey(const int index) const throw()
{
    std::map<std::string, std::map<std::string, int> >::const_iterator iter = data.begin();
    int iterIndex = 0;

    while(iter != data.end())
    {
        iter++;
        iterIndex++;
    }

    if(iterIndex == index)
    {
        return iter->first;
    }
    else
    {
        throw std::out_of_range("Graph index out of range");
    }
}

#endif