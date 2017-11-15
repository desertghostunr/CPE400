#ifndef DEVICE_DEF
#define DEVICE_DEF

#include <string>

class Device 
{
public:
	Device();

	void setDepart(int time);
	int getElapsedTime() const;
	int getDestination() const;
private:
	int id;
	int sourceAddress;
	int destAddress;
	int departTime;
};

#endif