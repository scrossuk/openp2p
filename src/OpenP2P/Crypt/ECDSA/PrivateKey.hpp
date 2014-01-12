#ifndef OPENP2P_CRYPT_ECDSA_PRIVATEKEY_HPP
#define OPENP2P_CRYPT_ECDSA_PRIVATEKEY_HPP

#include <stdint.h>

#include <stdexcept>

#include <cryptopp/eccrypto.h>
#include <cryptopp/ecp.h>
#include <cryptopp/sha.h>

#include <OpenP2P/Crypt/RandomPool.hpp>
#include <OpenP2P/Crypt/ECDSA/Curve.hpp>

namespace OpenP2P {

	namespace Crypt {
	
		namespace ECDSA {
		
			class PrivateKey {
				public:
					inline PrivateKey(RandomPool& pool, Curve curve) {
						CryptoPP::OID oid;
						
						switch (curve) {
							case brainpoolP256r1:
								oid = CryptoPP::ASN1::brainpoolP256r1();
								break;
								
							case brainpoolP512r1:
								oid = CryptoPP::ASN1::brainpoolP512r1();
								break;
								
							default:
								oid = CryptoPP::ASN1::brainpoolP256r1();
						}
						
						privateKey_.Initialize(pool, oid);
						
						if (!privateKey_.Validate(pool, 3)) {
							throw std::runtime_error("Generated private key is invalid.");
						}
					}
					
					inline operator CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::PrivateKey& () {
						return privateKey_;
					}
					
					inline operator const CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::PrivateKey& () const {
						return privateKey_;
					}
					
				private:
					CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::PrivateKey privateKey_;
					
			};
			
		}
		
	}
	
}

#endif
