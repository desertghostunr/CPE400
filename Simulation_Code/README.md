# CPE400
A repository for the CPE400 Networking Programming Project

## Building and Running on Linux

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
### Network Structure
### Vehicles
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

### Central Compute Node
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
The structure of the input file is fairly straight forward. Each object on the network has its own keyword that is recognized by the program:

* Car:

    * This keyword is used to specify a vehicle on the network.
    * car car-name source destination
* Intersect:

    * The intersection command specified a node within the network.
    * The command must contain an intersect name, as well as the maximum capacity of the node.
    * intersect intersect-name capacity

* Neighbor:
    
    * The neighbor keyword details a neighbor node to the preceding intersect.
    * The referenced neighbor node can be detailed either through a direct reference to the object, or by its index.
    * Along with a reference to the neighbor node, the length of time between the nodes must also be specified.
    * neighbor [node-index]|node-title timespan

The input file also allows for the use of comments, which begin with '#' at the beginning of the comment.
