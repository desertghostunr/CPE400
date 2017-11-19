#ifndef VEHICLE_H
#define VEHICLE_H

#include <list>
#include "CentralComputeNode.h"

class Vehicle 
{
	public:
		Vehicle();
    	~Vehicle();

		void setDepart(int currTime);
		int getElapsedTime() const;
		int getDestination() const;

		void requestRoute(const CentralComputeNode & ccn);
		void setRoute(std::list<int> route);

	private:
		int id;
		int sourceAddress;
		int destAddress;
		int departTime;

		std::list<int> route;
};

#endif