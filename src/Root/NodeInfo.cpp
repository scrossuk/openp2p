#include <vector>

#include <p2p/Stream/BinaryStream.hpp>

#include <p2p/Root/Endpoint.hpp>
#include <p2p/Root/NodeId.hpp>
#include <p2p/Root/NodeInfo.hpp>

namespace p2p {

	namespace Root {
		
		NodeInfo NodeInfo::Read(BlockingReader& reader) {
			const auto nodeId = NodeId::FromReader(reader);
			const auto endpointCount = Binary::ReadUint16(reader);
			
			std::vector<Endpoint> nodeEndpoints;
			for (uint16_t i = 0; i < endpointCount; i++) {
				nodeEndpoints.push_back(Endpoint::Read(reader));
			}
			
			return NodeInfo(nodeId, std::move(nodeEndpoints));
		}
		
		void NodeInfo::writeTo(BlockingWriter& writer) const {
			id.writeTo(writer);
			Binary::WriteUint16(writer, endpointList.size());
			for (const auto& endpoint: endpointList) {
				endpoint.writeTo(writer);
			}
		}
		
	}
	
}

