#include <stdint.h>

#include <memory>

#include <p2p/Transport.hpp>
#include <p2p/Transport/Stream.hpp>

#include <p2p/Crypt/ECDSA/PublicKey.hpp>
#include <p2p/Crypt/ECDSA/VerifyStream.hpp>

namespace p2p {

	namespace Crypt {
	
		namespace ECDSA {
		
			struct VerifyStreamImpl {
				bool isValid;
				CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::Verifier verifier;
				CryptoPP::SignatureVerificationFilter filter;
				
				VerifyStreamImpl(const PublicKey& publicKey)
					: isValid(false),
					verifier(publicKey),
					filter(verifier, new CryptoPP::ArraySink((byte*) &isValid, sizeof(isValid))) { }
			};
			
			VerifyStream::VerifyStream(const PublicKey& publicKey, const Buffer& signature)
				: impl_(new VerifyStreamImpl(publicKey)) {
				impl_->filter.Put(signature.data(), signature.size());
			}
			
			VerifyStream::~VerifyStream() { }
			
			bool VerifyStream::isValid() const {
				return true;
			}
			
			Event::Source VerifyStream::eventSource() const {
				return Event::Source();
			}
			
			size_t VerifyStream::write(const uint8_t* data, size_t size) {
				// Apparently this always returns 0,
				// which means success...
				(void) impl_->filter.Put((byte*) data, size);
				return size;
			}
			
			bool VerifyStream::isSignatureValid() {
				impl_->filter.MessageEnd();
				return impl_->isValid;
			}
			
		}
		
	}
	
}

