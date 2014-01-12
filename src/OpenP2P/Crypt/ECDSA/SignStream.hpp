#ifndef OPENP2P_CRYPT_ECDSA_SIGNSTREAM_HPP
#define OPENP2P_CRYPT_ECDSA_SIGNSTREAM_HPP

#include <stdint.h>

#include <string>

#include <cryptopp/eccrypto.h>
#include <cryptopp/ecp.h>
#include <cryptopp/filters.h>
#include <cryptopp/sha.h>

#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/Stream.hpp>

#include <OpenP2P/Crypt/RandomPool.hpp>
#include <OpenP2P/Crypt/ECDSA/PrivateKey.hpp>

namespace OpenP2P {

	namespace Crypt {
	
		namespace ECDSA {
		
			class SignStream: public OStream {
				public:
					inline SignStream(RandomPool& pool, const PrivateKey& privateKey)
						: signer_(privateKey) {
						
						filter_ = new CryptoPP::SignerFilter(pool, signer_, new CryptoPP::StringSink(signature_));
					}
					
					inline ~SignStream() {
						delete filter_;
					}
					
					inline bool isValid() const {
						return true;
					}
					
					inline size_t write(const uint8_t* data, size_t size) {
						return filter_->Put((byte*) data, size);
					}
					
					inline Buffer signature() {
						filter_->MessageEnd();
						
						return Buffer(signature_.begin(), signature_.end());
					}
					
				private:
					SignStream(const SignStream&) = delete;
					SignStream& operator=(SignStream) = delete;
					
					std::string signature_;
					CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::Signer signer_;
					CryptoPP::Filter* filter_;
					
			};
			
		}
		
	}
	
}

#endif
