# CPE400
A repository for the CPE400 Networking Programming Project

## Vehicles
Each vehicle on the network is an abstraction of a network packet. This allows each one to hold and share only basic information, such as identification, source, and destination addresses. The majority of routing of these packets is therefore completed by the intersection routers. The amount of time spend between each node is recorded by the packet, which is then read by the router and passed to the central device. This information helps the Central Node determine wait times at each individual node; this in turn allows it to scale and reroute packets accordingly.


Class Device:

	* Methods:

		* Constructor
		* Set Leave Time
		* Compute Time elapsed
		* Get Destination Address
	* Properties:

		* Device id
		* Source address
		* Dest address
		* Time leave node (UTC)

