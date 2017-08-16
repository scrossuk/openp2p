#include <stdint.h>

#include <stdexcept>

#include <cryptopp/eccrypto.h>
#include <cryptopp/ecp.h>
#include <cryptopp/sha.h>

#include <p2p/Crypt/RandomPool.hpp>
#include <p2p/Crypt/ECDSA/Curve.hpp>
#include <p2p/Crypt/ECDSA/PrivateKey.hpp>

namespace p2p {

	namespace Crypt {
	
		namespace ECDSA {
		
			PrivateKey::PrivateKey() { }
			
			PrivateKey::PrivateKey(RandomPool& pool, Curve curve) {
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
				
				while (true) {
					// Black box algorithm: keep generating keys
					// until y = min(y, p - y).
					privateKey_.Initialize(pool, oid);
					CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::PublicKey publicKey;
					privateKey_.MakePublicKey(publicKey);
					
					if (publicKey.GetPublicElement().y.GetBit(0) == 0) {
						break;
					}
				}
				
				if (!privateKey_.Validate(pool, 3)) {
					throw std::runtime_error("Generated private key is invalid.");
				}
			}
			
			PrivateKey::operator CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::PrivateKey& () {
				return privateKey_;
			}
			
			PrivateKey::operator const CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::PrivateKey& () const {
				return privateKey_;
			}
			
		}
		
	}
	
}

