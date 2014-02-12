#ifndef OPENP2P_CRYPT_ECDSA_PUBLICKEY_HPP
#define OPENP2P_CRYPT_ECDSA_PUBLICKEY_HPP

#include <cryptopp/eccrypto.h>
#include <cryptopp/ecp.h>
#include <cryptopp/sha.h>

#include <OpenP2P/Buffer.hpp>

namespace OpenP2P {

	namespace Crypt {
	
		class RandomPool;
		
		namespace ECDSA {
		
			class PrivateKey;
			
			class PublicKey {
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
