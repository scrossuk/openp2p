#ifndef P2P_CRYPT_ECDSA_PUBLICKEY_HPP
#define P2P_CRYPT_ECDSA_PUBLICKEY_HPP

#include <cryptopp/eccrypto.h>
#include <cryptopp/ecp.h>
#include <cryptopp/sha.h>

#include <p2p/Crypt/Crypt_api.hpp>
#include <p2p/Transport.hpp>

namespace p2p {

	namespace Crypt {
	
		class RandomPool;
		
		namespace ECDSA {
		
			class PrivateKey;
			
			class CRYPT_OPENP2P_API PublicKey {
				public:
					PublicKey();
					PublicKey(RandomPool& pool, const PrivateKey& privateKey);
					
					static PublicKey FromBuffer(const Buffer& buffer);
					
					Buffer toBuffer() const;
					
					operator CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::PublicKey& ();
					
					operator const CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::PublicKey& () const;
					
				private:
					CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::PublicKey key_;
					
			};
			
		}
		
	}
	
}

#endif
