#ifndef OPENP2P_CRYPT_ECDSA_PUBLICKEY_HPP
#define OPENP2P_CRYPT_ECDSA_PUBLICKEY_HPP

#include <cstddef>

#include <cryptopp/eccrypto.h>
#include <cryptopp/ecp.h>
#include <cryptopp/sha.h>

#include <OpenP2P/Crypt/RandomPool.hpp>
#include <OpenP2P/Crypt/ECDSA/PrivateKey.hpp>

namespace OpenP2P {

	namespace Crypt {
	
		namespace ECDSA {
		
			class PublicKey {
				public:
					inline PublicKey() { }
					
					inline PublicKey(RandomPool& pool, const PrivateKey& privateKey) {
						((CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::PrivateKey&) privateKey).MakePublicKey(publicKey_);
						
						if (!publicKey_.Validate(pool, 3)) {
							throw std::runtime_error("Generated private key is invalid.");
						}
					}
					
					inline operator CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::PublicKey& () {
						return publicKey_;
					}
					
					inline operator const CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::PublicKey& () const {
						return publicKey_;
					}
					
				private:
					CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::PublicKey publicKey_;
					
			};
			
		}
		
	}
	
}

#endif
