#ifndef VEHICLE_H
#define VEHICLE_H

#include <list>
#include "CentralComputeNode.h"

// we need to make this thread safe

// maybe something like this? https://stackoverflow.com/questions/33895071/how-to-implement-class-lock-objects-for-multithreaded-access

// the second one looks option in the solution might be better

// I wish c++ had something as simple as lock(){} in c# ... 

class Vehicle 
{
    using Lock = std::unique_lock<std::mutex>;

	public:
		Vehicle();
    	~Vehicle();

		void setDepart(int currTime);
		int getElapsedTime() const;
		int getDestination() const;

		void requestRoute(const CentralComputeNode & ccn);
		void setRoute(std::list<int> route);

        void notifyRoadChange(const CentralComputeNode & cnn);

        Lock getLock();

        void releaseLock();

	private:
		int id;
		int sourceAddress;
		int destAddress;
		int departTime;

		std::list<int> route;

        mutable std::mutex mutex;
};

#endif