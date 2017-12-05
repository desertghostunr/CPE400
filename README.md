# CPE400
A repository for the CPE400 Networking Programming Project

## Building and Running

### Building and Running on Linux

Building:

```bash
make
```

Running:

```bash
./SDN Input.txt
```

Cleaning:

```bash
make clean
```

## Vehicles
Each vehicle on the network is an abstraction of a network packet. This allows each one to hold and share only basic information, such as identification, source, and destination addresses. The majority of routing of these packets is therefore completed by the intersection routers. The amount of time spend between each node is recorded by the packet, which is then read by the router and passed to the central device. This information helps the Central Node determine wait times at each individual node; this in turn allows it to scale and reroute packets accordingly.


Class Vehicle:

	* Methods:

		* Constructor
		* Parameterized Constructor
		* Destructor
		* Set Start Time
		* Set Depart Time
		* Get Travel Time
		* Get Total Time
		* Get Next Destination
		* Time Remaining To Next Destination
		* Clear Route
		* Has Route
		* Has Node
		* Get ID
		* Get Source
		* Get Dest
		* Request Route
		* Set Route
		* Try Road Change
		* Get Lock
		* Release Lock

	* Properties:

		* Device id
		* Source address
		* Dest address
		* Travel Time
		* Total Time
		* Travel Time Left
		* Route (a list of subnets to traverse)
		* Route Requested
		* mutex

## Central Compute Node
The central compute node is responsible for routing all traffic.

Class CentralComputeNode:

	* Methods:

		* Constructor
		* Destructor
		* Build Subnet To Index Table
		* Get Map Index
		* Set Map
		* Set Subnet Properties
		* Queue Job
		* Compute Route
		* Direct Traffic
		* Join Network
		* Leave Network
		* Change Road
		* Get Lock
		* Release Lock
		* AStar
		* Reconstruct Path
		* Expand Node

	* Properties:

		* Vehicle ID to Vehicle Object (the abstracted "route" to that vehicle)
		* Subnet Capacity
		* Vehicles at each subnet (map)
		* City Map (adjacency matrix)
		* Subnet To Index Table
		* Jobs (a queue of routes to be computed)

		* mutex

## Input Structure
* Please see the Input.txt for notes on the input structure.
