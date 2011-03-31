#ifndef OPENP2P_ROOTNETWORK_NODE_HPP
#define OPENP2P_ROOTNETWORK_NODE_HPP

#include <OpenP2P/Kademlia/Node.hpp>

#include <OpenP2P/RootNetwork/Endpoint.hpp>
#include <OpenP2P/RootNetwork/Parameters.hpp>

namespace OpenP2P{

	namespace RootNetwork{

		typedef typename Kademlia::Node<Endpoint, IdSize> Node;

	}

}

#endif
