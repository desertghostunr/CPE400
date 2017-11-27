#ifndef VEHICLE_H
#define VEHICLE_H

#include <list>
#include <string>
#include "ThreadSafeObject.h"
#include "CentralComputeNode.h"

// we need to make this thread safe

// maybe something like this? https://stackoverflow.com/questions/33895071/how-to-implement-class-lock-objects-for-multithreaded-access

// the second one looks option in the solution might be better

// I wish c++ had something as simple as lock(){} in c# ... 

class Vehicle : public ThreadSafeObject
{
	public:
		Vehicle(std::string newID, std::string newSource, std::string newDest);
    	~Vehicle();

		int getTravelTime() const;
        int getTotalTime() const;
		std::string getNextDestination() const;

        bool hasRoute() const;
        bool hasNode(const std::string &node) const;
		
        void requestRoute(const CentralComputeNode & ccn);
		void setRoute(std::list<std::pair<std::string, long>> &route);

        void tryRoadChange(const CentralComputeNode & cnn);

        bool onRoute();


	private:
		std::string id;
		std::string sourceAddress;
	    std::string destAddress;
		int travelTime;
        int totalTime;

		std::list<std::pair<std::string, long>>* route;
};

#endif