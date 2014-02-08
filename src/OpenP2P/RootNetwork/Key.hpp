#ifndef OPENP2P_ROOTNETWORK_KEY_HPP
#define OPENP2P_ROOTNETWORK_KEY_HPP

#include <OpenP2P/Crypt/ECDSA/PrivateKey.hpp>
#include <OpenP2P/Crypt/ECDSA/PublicKey.hpp>

namespace OpenP2P {

	namespace RootNetwork {
	
		using PrivateKey = Crypt::ECDSA::PrivateKey;
		using PublicKey = Crypt::ECDSA::PublicKey;
		
	}
	
}

#endif
