#ifndef OPENP2P_ROOTNETWORK_RPCWRAPPER_HPP
#define OPENP2P_ROOTNETWORK_RPCWRAPPER_HPP

#include <OpenP2P/BinaryStream.hpp>

#include <OpenP2P/RootNetwork/Id.hpp>

namespace OpenP2P{

	namespace RootNetwork{

		//Frame for all rpc requests and replies
		template <typename RPCType>
		struct RPCWrapper{
			Id senderId;
			RPCType rpc;
		};

		template <typename RPCType>
		BinaryStream& operator>>(BinaryStream& stream, RPCWrapper<RPCType>& wrapper){
			return stream >> wrapper.senderId >> wrapper.rpc;
		}

		template <typename RPCType>
		BinaryStream& operator<<(BinaryStream& stream, const RPCWrapper<RPCType>& wrapper){
			return stream << wrapper.senderId << wrapper.rpc;
		}

	}

}

#endif
