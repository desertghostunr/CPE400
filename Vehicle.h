#ifndef VEHICLE_H
#define VEHICLE_H

#include <list>
#include "ThreadSafeObject.h"
#include "CentralComputeNode.h"

// we need to make this thread safe

// maybe something like this? https://stackoverflow.com/questions/33895071/how-to-implement-class-lock-objects-for-multithreaded-access

// the second one looks option in the solution might be better

// I wish c++ had something as simple as lock(){} in c# ... 

class Vehicle : public ThreadSafeObject
{
	public:
		Vehicle();
    	~Vehicle();

		void setDepart(int currTime);
		int getElapsedTime() const;
		int getDestination() const;

		void requestRoute(const CentralComputeNode & ccn);
		void setRoute(std::list<std::pair<int, long long > > route);

        void notifyRoadChange(const CentralComputeNode & cnn);

        bool onRoute();

        bool hasRoute();

	private:
		int id;
		int sourceAddress;
		int destAddress;
		int departTime;

		std::list<int> route;
};

#endif