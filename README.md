# CPE400
A repository for the CPE400 Networking Programming Project

## Vehicles
Each vehicle on the network is an abstraction of a network packet. This allows each one to hold and share only basic information, such as identification, source, and destination addresses. The majority of routing of these packets is therefore completed by the intersection routers. The amount of time spend between each node is recorded by the packet, which is then read by the router and passed to the central device. This information helps the Central Node determine wait times at each individual node; this in turn allows it to scale and reroute packets accordingly.


Class Vehicle:

	* Methods:

		* Constructor
		* Destructor
		* Set Leave Time
		* Compute Time elapsed
		* Get Destination Address

		* Request Route

		* Set Route

		* Notify Road Change

		* Get Lock

		* Release Lock

	* Properties:

		* Device id
		* Source address
		* Dest address
		* Time leave node (UTC)

		* Route (a list of subnets to traverse)

		* mutex

## Central Compute Node
The central compute node is responsible for routing all traffic.

Class CentralComputeNode:

	* Methods:

		* Constructor
		* Destructor
		* Queue Job
		* Compute Route
		* Direct Traffic
		* Join Network
		* Get Lock
		* Release Lock

	* Properties:

		* Vehicle ID to Vehicle Object (the abstracted "route" to that vehicle)
		* Subnet Speed
		* Subnet Capacity
		* Vehicles at each subnet (map)
		* Vehicles going to each subnet (map)
		* City Map (adjacency matrix)
		* Jobs (a queue of routes to be computed)

		* mutex
