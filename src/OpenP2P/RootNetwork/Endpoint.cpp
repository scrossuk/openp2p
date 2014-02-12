#include <OpenP2P/TCP/Endpoint.hpp>
#include <OpenP2P/UDP/Endpoint.hpp>

#include <OpenP2P/RootNetwork/Endpoint.hpp>

namespace OpenP2P {

	namespace RootNetwork {
	
		Endpoint::Endpoint()
			: kind(LOCAL) { }
		
		Endpoint::Endpoint(const UDP::Endpoint& pUdpEndpoint)
			// TODO: detect this.
			: kind(UDPIPV6),
			udpEndpoint(pUdpEndpoint) { }
		
		Endpoint Endpoint::Read(BlockingReader& reader) {
			Endpoint endpoint;
			endpoint.kind = static_cast<Kind>(Binary::ReadUint8(reader));
			switch (endpoint.kind) {
				case LOCAL:
					break;
				case UDPIPV4:
				{
					IP::V4Address address;
					reader.readAll(address.data.data(), address.data.size());
					const uint16_t port = Binary::ReadUint16(reader);
					endpoint.udpEndpoint = UDP::Endpoint(IP::Address(address), port);
					break;
				}
				case UDPIPV6:
				{
					IP::V6Address address;
					reader.readAll(address.data.data(), address.data.size());
					const uint16_t port = Binary::ReadUint16(reader);
					endpoint.udpEndpoint = UDP::Endpoint(IP::Address(address), port);
					break;
				}
				case TCPIPV4:
				{
					IP::V4Address address;
					reader.readAll(address.data.data(), address.data.size());
					const uint16_t port = Binary::ReadUint16(reader);
					endpoint.tcpEndpoint = TCP::Endpoint(IP::Address(address), port);
					break;
				}
				case TCPIPV6:
				{
					IP::V6Address address;
					reader.readAll(address.data.data(), address.data.size());
					const uint16_t port = Binary::ReadUint16(reader);
					endpoint.tcpEndpoint = TCP::Endpoint(IP::Address(address), port);
					break;
				}
				default:
					break;	
			}
			return endpoint;
		}
			
		void Endpoint::writeTo(BlockingWriter& writer) const {
			Binary::WriteUint8(writer, static_cast<uint8_t>(kind));
			switch (kind) {
				case LOCAL:
					break;
				case UDPIPV4:
				{
					const auto& address = udpEndpoint.address.v4Address;
					writer.writeAll(address.data.data(), address.data.size());
					Binary::WriteUint16(writer, udpEndpoint.port);
					break;
				}
				case UDPIPV6:
				{
					const auto& address = udpEndpoint.address.v6Address;
					writer.writeAll(address.data.data(), address.data.size());
					Binary::WriteUint16(writer, udpEndpoint.port);
					break;
				}
				case TCPIPV4:
				{
					const auto& address = tcpEndpoint.address.v4Address;
					writer.writeAll(address.data.data(), address.data.size());
					Binary::WriteUint16(writer, tcpEndpoint.port);
					break;
				}
				case TCPIPV6:
				{
					const auto& address = tcpEndpoint.address.v6Address;
					writer.writeAll(address.data.data(), address.data.size());
					Binary::WriteUint16(writer, tcpEndpoint.port);
					break;
				}
				default:
					break;	
			}
		}
		
	}
	
}

