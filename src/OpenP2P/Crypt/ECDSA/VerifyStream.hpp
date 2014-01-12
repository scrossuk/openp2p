#ifndef OPENP2P_CRYPT_ECDSA_VERIFYSTREAM_HPP
#define OPENP2P_CRYPT_ECDSA_VERIFYSTREAM_HPP

#include <stdint.h>

#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/Stream.hpp>

#include <OpenP2P/Crypt/ECDSA/PublicKey.hpp>

namespace OpenP2P {

	namespace Crypt {
	
		namespace ECDSA {
		
			class VerifyStream: public OStream {
				public:
					inline VerifyStream(const PublicKey& publicKey, const Buffer& signature)
						: verifier_(publicKey), isValid_(false) {
						
						filter_ = new CryptoPP::SignatureVerificationFilter(
							verifier_,
							new CryptoPP::ArraySink((byte*) &isValid_, sizeof(isValid_))
						);
						filter_->Put(&signature[0], signature.size());
					}
					
					inline ~VerifyStream() {
						delete filter_;
					}
					
					inline bool isValid() const {
						return true;
					}
					
					inline Event::Source eventSource() const {
						return Event::Source();
					}
					
					inline size_t write(const uint8_t* data, size_t size) {
						return filter_->Put((byte*) data, size);
					}
					
					inline bool isSignatureValid() {
						filter_->MessageEnd();
						return isValid_;
					}
					
				private:
					CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::Verifier verifier_;
					CryptoPP::Filter* filter_;
					bool isValid_;
					
			};
			
		}
		
	}
	
}

#endif
