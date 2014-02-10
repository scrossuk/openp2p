#ifndef OPENP2P_CRYPT_ECDSA_PUBLICKEY_HPP
#define OPENP2P_CRYPT_ECDSA_PUBLICKEY_HPP

#include <cstddef>

#include <cryptopp/eccrypto.h>
#include <cryptopp/ecp.h>
#include <cryptopp/sha.h>

#include <OpenP2P/Buffer.hpp>

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
					
					inline static PublicKey FromBuffer(const Buffer& buffer) {
						PublicKey publicKey;
						publicKey.publicKey_.AccessGroupParameters().AssignFrom(CryptoPP::ASN1::brainpoolP256r1());
						
						CryptoPP::ByteQueue byteQueue;
						
						// Generate DER compressed format
						// where y = min(y, p - y).
						byteQueue.Put(0x02);
						byteQueue.Put(buffer.data(), buffer.size());
						
						publicKey.publicKey_.BERDecodePublicKey(byteQueue, false, 32);
						return publicKey;
					}
					
					inline Buffer toBuffer() const {
						CryptoPP::ByteQueue byteQueue;
						publicKey.publicKey_.DEREncodePublicKey(byteQueue);
						
						uint8_t formatByte = 0;
						byteQueue.Get(formatByte);
						assert(formatByte == 0x02);
						
						Buffer buffer(32, 0x00);
						byteQueue.get(buffer.data(), buffer.size());
						return buffer;
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
