#ifndef OPENP2P_IP_ENDPOINT_HPP
#define OPENP2P_IP_ENDPOINT_HPP

#include <stdint.h>
#include <OpenP2P/BinaryStream.hpp>
#include <OpenP2P/IP/Address.hpp>

namespace OpenP2P{

	namespace IP{

		struct Endpoint{
			Address address;
			uint16_t port;
		};

	}

}

#endif
