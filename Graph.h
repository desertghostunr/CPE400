#ifndef GRAPH_H
#define GRAPH_H

#include <map>
#include <vector>
#include <string>
#include <utility>

class Graph
{
    public:
        Graph();
        ~Graph();

        void addNeighbor(const std::string &key, const std::string &neighbor, const int time);

        std::map<std::string, int>& operator[](const std::string &key);
        std::map<std::string, int>& operator[](const int index) throw();

        std::string getKey(const int index) const throw();
    
    private:
        std::map<std::string, std::map<std::string, int> > data;
        std::map<std::string, int> adjacentNodes;

};
#endif