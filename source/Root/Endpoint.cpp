#include <p2p/TCP/Endpoint.hpp>
#include <p2p/UDP/Endpoint.hpp>

#include <p2p/Root/Endpoint.hpp>

#include <p2p/Util/String.hpp>

namespace p2p {

	namespace Root {
	
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
		
		bool Endpoint::operator<(const Endpoint& endpoint) const {
			if (kind != endpoint.kind) {
				return kind < endpoint.kind;
			}
			
			switch (kind) {
				case LOCAL:
					return false;
				case UDPIPV4:
				case UDPIPV6:
					return udpEndpoint < endpoint.udpEndpoint;
				case TCPIPV4:
				case TCPIPV6:
					return tcpEndpoint < endpoint.tcpEndpoint;
				default:
					return false;
			}
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
		
		std::string Endpoint::toString() const {
			switch (kind) {
				case LOCAL:
					return "Root::Endpoint(kind = LOCAL)";
				case UDPIPV4:
					return STR("Root::Endpoint(kind = UDPIPv4, value = %s)", udpEndpoint.toString().c_str());
				case UDPIPV6:
					return STR("Root::Endpoint(kind = UDPIPv6, value = %s)", udpEndpoint.toString().c_str());
				case TCPIPV4:
					return STR("Root::Endpoint(kind = TCPIPv4, value = %s)", tcpEndpoint.toString().c_str());
				case TCPIPV6:
					return STR("Root::Endpoint(kind = TCPIPv6, value = %s)", tcpEndpoint.toString().c_str());
				default:
					return "Root::Endpoint(kind = [UNKNOWN])";
			}
		}
		
	}
	
}

