#ifndef OPENP2P_ROOTNETWORK_RPCWRAPPER_HPP
#define OPENP2P_ROOTNETWORK_RPCWRAPPER_HPP

#include <OpenP2P/BinaryStream.hpp>

#include <OpenP2P/RootNetwork/Id.hpp>
#include <OpenP2P/RootNetwork/RPCType.hpp>

namespace OpenP2P{

	namespace RootNetwork{

		//Frame for all rpc requests and replies
		template <typename RPC>
		struct RPCWrapper{
			Id senderId;
			RPCType type;
			RPC rpc;

			inline RPCWrapper(){ }

			inline RPCWrapper(const Id& i, RPCType t, const RPC& r) : senderId(i), type(t), rpc(r){ }
		};

		template <typename RPCType>
		BinaryStream& operator>>(BinaryStream& stream, RPCWrapper<RPCType>& wrapper){
			return stream >> wrapper.senderId >> wrapper.type >> wrapper.rpc;
		}

		template <typename RPCType>
		BinaryStream& operator<<(BinaryStream& stream, const RPCWrapper<RPCType>& wrapper){
			return stream << wrapper.senderId << wrapper.type << wrapper.rpc;
		}

	}

}

#endif
