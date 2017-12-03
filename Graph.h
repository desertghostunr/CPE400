#ifndef GRAPH_H
#define GRAPH_H

#include <map>
#include <string>

class Graph
{
    public:
        Graph();
        ~Graph();

        bool addNode(const std::string &newKey, const std::map<std::string, int> &neighbors);

        std::map<std::string, int>& operator[](const std::string &key);
        std::map<std::string, int>& operator[](const int index);

    private:
        std::map<std::string, std::map<std::string, int> > data;
};
#endif