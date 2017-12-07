all: main.cpp Vehicle.o CentralComputeNode.o ThreadSafeObject.o
	g++ -std=c++11 -o SDN main.cpp Vehicle.o CentralComputeNode.o ThreadSafeObject.o -lpthread
Vehicle.o: Vehicle.cpp Vehicle.h CentralComputeNode.h ThreadSafeObject.h
	g++ -std=c++11 -c -Wall Vehicle.cpp
CentralComputeNode.o: CentralComputeNode.cpp CentralComputeNode.h Vehicle.h ThreadSafeObject.h
	g++ -std=c++11 -c -Wall CentralComputeNode.cpp
ThreadSafeObject.o: ThreadSafeObject.cpp ThreadSafeObject.h
	g++ -std=c++11 -c -Wall ThreadSafeObject.cpp
clean:
	rm -f *.o SDN