#ifndef OPENP2P_CRYPT_ECDSA_PRIVATEKEY_HPP
#define OPENP2P_CRYPT_ECDSA_PRIVATEKEY_HPP

#include <cryptopp/eccrypto.h>
#include <cryptopp/ecp.h>
#include <cryptopp/sha.h>

#include <OpenP2P/Crypt/ECDSA/Curve.hpp>

namespace OpenP2P {

	namespace Crypt {
	
		class RandomPool;
		
		namespace ECDSA {
		
			class PrivateKey {
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
