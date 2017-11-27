#ifndef VEHICLE_H
#define VEHICLE_H

#include <list>
#include <string>
#include "ThreadSafeObject.h"
#include "CentralComputeNode.h"


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

        std::string getID();
        std::string getSource();
        std::string getDest();
		
        void requestRoute(CentralComputeNode & ccn);
		void setRoute(std::list<std::pair<std::string, long long>> newRoute);

        bool tryRoadChange(CentralComputeNode & ccn);

        bool onRoute();


	private:
		std::string id;
		std::string sourceAddress;
	    std::string destAddress;
		int travelTime;
        int totalTime;

		std::list<std::pair<std::string, long long>>* route;
};

#endif