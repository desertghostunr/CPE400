#ifndef DEVICE_DEF
#define DEVICE_DEF

#include <list>

class Device 
{
public:
	Device();
    ~Device();

	void setDepart(int time);
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