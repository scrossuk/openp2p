#ifndef P2P_CRYPT_ECDSA_PRIVATEKEY_HPP
#define P2P_CRYPT_ECDSA_PRIVATEKEY_HPP

#include <cryptopp/eccrypto.h>
#include <cryptopp/ecp.h>
#include <cryptopp/sha.h>

#include <p2p/Crypt/Crypt_api.hpp>
#include <p2p/Crypt/ECDSA/Curve.hpp>

namespace p2p {

	namespace Crypt {
	
		class RandomPool;
		
		namespace ECDSA {
		
			class CRYPT_OPENP2P_API PrivateKey {
				public:
					PrivateKey();
					PrivateKey(RandomPool& pool, Curve curve);
					
					operator CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::PrivateKey& ();
					
					operator const CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::PrivateKey& () const;
					
				private:
					CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::PrivateKey privateKey_;
					
			};
			
		}
		
	}
	
}

#endif
