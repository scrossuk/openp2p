#include <stddef.h>

#include <stdexcept>

#include <cryptopp/eccrypto.h>
#include <cryptopp/ecp.h>
#include <cryptopp/sha.h>

#include <p2p/Transport.hpp>

#include <p2p/Crypt/RandomPool.hpp>
#include <p2p/Crypt/ECDSA/PublicKey.hpp>
#include <p2p/Crypt/ECDSA/PrivateKey.hpp>

namespace p2p {

	namespace Crypt {
	
		namespace ECDSA {
		
			PublicKey::PublicKey() {
				key_.AccessGroupParameters().SetPointCompression(true);
			}
			
			PublicKey::PublicKey(RandomPool& pool, const PrivateKey& privateKey) {
				((CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::PrivateKey&) privateKey).MakePublicKey(key_);
				
				if (!key_.Validate(pool, 3)) {
					throw std::runtime_error("Generated private key is invalid.");
				}
				
				key_.AccessGroupParameters().SetPointCompression(true);
			}
			
			PublicKey PublicKey::FromBuffer(const Buffer& buffer) {
				PublicKey publicKey;
				publicKey.key_.Initialize(CryptoPP::ASN1::brainpoolP256r1(), decltype(publicKey.key_)::Element());
				publicKey.key_.AccessGroupParameters().SetPointCompression(true);
				
				CryptoPP::ByteQueue byteQueue;
				
				// Generate DER compressed format
				// where y = min(y, p - y).
				byteQueue.Put(0x02);
				byteQueue.Put(buffer.data(), buffer.size());
				
				publicKey.key_.BERDecodePublicKey(byteQueue, false, 1 + 32);
				return publicKey;
			}
			
			Buffer PublicKey::toBuffer() const {
				CryptoPP::ByteQueue byteQueue;
				key_.DEREncodePublicKey(byteQueue);
				
				uint8_t formatByte = 0;
				byteQueue.Get(formatByte);
				assert(formatByte == 0x02);
				
				Buffer buffer(32, 0x00);
				byteQueue.Get(buffer.data(), buffer.size());
				return buffer;
			}
			
			PublicKey::operator CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::PublicKey& () {
				return key_;
			}
			
			PublicKey::operator const CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::PublicKey& () const {
				return key_;
			}
			
		}
		
	}
	
}

