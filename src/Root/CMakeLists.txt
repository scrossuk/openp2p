add_library(openp2p-rootnetwork STATIC
	AuthenticatedSocket.cpp
	Endpoint.cpp
	EndpointMapSocket.cpp
	NetworkId.cpp
	NodeDatabase.cpp
	NodeId.cpp
	Packet.cpp
	PacketSocket.cpp
	PrivateIdentity.cpp
	PublicIdentity.cpp
	RoutineIdGenerator.cpp
	TransportSocket.cpp
	
	Core/RPCMessage.cpp
	Core/RPCService.cpp
	
	DHT/RPCMessage.cpp
	DHT/RPCService.cpp
	)
