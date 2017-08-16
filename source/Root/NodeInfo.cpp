#include <vector>

#include <p2p/Root/Endpoint.hpp>
#include <p2p/Root/NodeId.hpp>
#include <p2p/Root/NodeInfo.hpp>

#include <p2p/Transport.hpp>

namespace p2p {

	namespace Root {
		
		NodeInfo NodeInfo::Read(BlockingReader& reader) {
			const auto nodeId = NodeId::FromReader(reader);
			const auto endpointCount = Binary::ReadUint16(reader);
			
			std::set<Endpoint> nodeEndpoints;
			for (uint16_t i = 0; i < endpointCount; i++) {
				nodeEndpoints.insert(Endpoint::Read(reader));
			}
			
			return NodeInfo(nodeId, std::move(nodeEndpoints));
		}
		
		void NodeInfo::writeTo(BlockingWriter& writer) const {
			id.writeTo(writer);
			Binary::WriteUint16(writer, endpointSet.size());
			for (const auto& endpoint: endpointSet) {
				endpoint.writeTo(writer);
			}
		}
		
	}
	
}

