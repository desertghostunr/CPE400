/**
 * @file Vehicle.h
 * 
 * @brief   Definition file for the Vehicle class
 * @details This class is the object that represents all vehicles on the network.
 * 
 * @author  Andrew Frost, Richard Millar
 * @version 1.00
 */

#ifndef VEHICLE_H
#define VEHICLE_H

#include <list>
#include <string>
#include <chrono>
#include "ThreadSafeObject.h"
#include "CentralComputeNode.h"

class CentralComputeNode;

class Vehicle : public ThreadSafeObject
{
	public:
        Vehicle();
		Vehicle(std::string newID, std::string newSource, std::string newDest);
        Vehicle(const Vehicle & other);
    	~Vehicle();

        void setStartTime();
        void setDepartTime();

        std::chrono::duration<double> getTravelTime() const;
        std::chrono::duration<double> getTotalTime() const;
		std::string getNextDestination() const;

        bool timeRemainingToNextDestination() const;

        void clearRoute();

        bool hasRoute() const;
        bool hasNode(const std::string &node) const;

        std::string getID();
        std::string getSource();
        std::string getDest();
		
        void requestRoute(CentralComputeNode & ccn);
		void setRoute(std::list<std::pair<std::string, double>> newRoute);

        bool tryRoadChange(CentralComputeNode & ccn);


	private:
		std::string id;
		std::string sourceAddress;
	    std::string destAddress;
        std::chrono::time_point<std::chrono::system_clock> travelTime;
        std::chrono::time_point<std::chrono::system_clock> totalTime;

        double travelTimeLeft;

		std::list<std::pair<std::string, double>>* route;

        bool routeRequested;
};

#endif