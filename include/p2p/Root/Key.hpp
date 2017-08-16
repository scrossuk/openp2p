#ifndef P2P_ROOTNETWORK_KEY_HPP
#define P2P_ROOTNETWORK_KEY_HPP

#include <p2p/Root/Root_api.hpp>
#include <p2p/Crypt/ECDSA/PrivateKey.hpp>
#include <p2p/Crypt/ECDSA/PublicKey.hpp>

namespace p2p {

	namespace Root {
	
		using PrivateKey = Crypt::ECDSA::PrivateKey;
		using PublicKey = Crypt::ECDSA::PublicKey;
		
	}
	
}

#endif
