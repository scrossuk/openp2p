#include <stdint.h>
#include <stdio.h>

#include <OpenP2P.hpp>
#include <OpenP2P/Crypt/AutoSeededRandomPool.hpp>
#include <OpenP2P/MultiplexSocket.hpp>
#include <OpenP2P/RootNetwork.hpp>
#include <OpenP2P/UDP.hpp>

using namespace OpenP2P;

class EventThread: public Runnable {
	public:
		EventThread(Socket<RootNetwork::Endpoint, RootNetwork::Packet>& socket)
			: service_(socket) { }
			
		void run() {
			service_.addNetwork("test");
			service_.addNetwork("p2p.rootdht");
			service_.processRequests();
		}
		
		void cancel() {
			// TODO...
		}
		
	private:
		RootNetwork::RPCService service_;
	
};

int main(int argc, char** argv) {
	if (argc != 3) {
		printf("Usage: client [my port] [other port]\n");
		return -1;
	}
	
	const auto myPort = atoi(argv[1]);
	const auto otherPort = atoi(argv[2]);
	
	UDP::Socket socket(myPort);
	
	Crypt::AutoSeededRandomPool rand;
	Crypt::ECDSA::PrivateKey privateKey(rand, Crypt::ECDSA::brainpoolP256r1);
	
	RootNetwork::PrivateIdentity privateIdentity(privateKey);
	
	RootNetwork::PacketSocket packetSocket(socket);
	
	RootNetwork::AuthenticatedSocket authenticatedSocket(privateIdentity, packetSocket);
	
	MultiplexHost<RootNetwork::Endpoint, RootNetwork::Packet> multiplexHost(authenticatedSocket);
	MultiplexClient<RootNetwork::Endpoint, RootNetwork::Packet> eventClient(multiplexHost);
	MultiplexClient<RootNetwork::Endpoint, RootNetwork::Packet> rpcClient(multiplexHost);
	
	EventThread eventThreadRunnable(eventClient);
	Thread eventThread(eventThreadRunnable);
	
	RootNetwork::RPCService service(rpcClient);
	
	const RootNetwork::NodeId nodeId = service.identifyEndpoint(UDP::Endpoint(IP::V4Address::Localhost(), otherPort));
	
	(void) nodeId;
	
	const auto endpoint = service.pingNode(UDP::Endpoint(IP::V4Address::Localhost(), otherPort), RootNetwork::NodeId());
	
	printf("My endpoint is '%s'.\n", endpoint.udpEndpoint.toString().c_str());
	
	const auto networks = service.queryNetworks(UDP::Endpoint(IP::V4Address::Localhost(), otherPort), RootNetwork::NodeId());
	
	printf("Node supports %llu networks.\n", (unsigned long long) networks.size());
	
	for (size_t i = 0; i < networks.size(); i++) {
		printf("    Network %llu: %s.\n", (unsigned long long) i, networks.at(i).hexString().c_str());
		if (networks.at(i) == RootNetwork::NetworkId::Generate("test")) {
			printf("         -> Supports 'test' network.\n");
		}
	}
	
	return 0;
}

