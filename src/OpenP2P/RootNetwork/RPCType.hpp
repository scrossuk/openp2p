#ifndef OPENP2P_ROOTNETWORK_RPCTYPE_HPP
#define OPENP2P_ROOTNETWORK_RPCTYPE_HPP

#include <OpenP2P/BinaryStream.hpp>
#include <OpenP2P/RootNetwork/Id.hpp>

namespace OpenP2P{

	namespace RootNetwork{
	
		enum RPCType{
			RPC_FINDNODE = 0,
			RPC_PING = 1,
			RPC_GETSUBSCRIBERS = 2,
			RPC_SUBSCRIBE = 3
		};

		inline BinaryStream& operator>>(BinaryStream& stream, RPCType& type){
			uint8_t t;
			stream >> t;
			type = (RPCType) t;
			return stream;
		}

		inline BinaryStream& operator<<(BinaryStream& stream, const RPCType& type){
			return stream << uint8_t(type);
		}
	
	}
	
}

#endif
