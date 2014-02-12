#include <stdint.h>

#include <memory>
#include <string>

#include <cryptopp/eccrypto.h>
#include <cryptopp/ecp.h>
#include <cryptopp/filters.h>
#include <cryptopp/sha.h>

#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/Stream.hpp>

#include <OpenP2P/Crypt/RandomPool.hpp>
#include <OpenP2P/Crypt/ECDSA/PrivateKey.hpp>
#include <OpenP2P/Crypt/ECDSA/SignStream.hpp>

namespace OpenP2P {

	namespace Crypt {
	
		namespace ECDSA {
		
			struct SignStreamImpl {
				std::string signature;
				CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::Signer signer;
				CryptoPP::SignerFilter filter;
				
				SignStreamImpl(RandomPool& pool, const PrivateKey& privateKey)
					: signer(privateKey),
					filter(pool, signer, new CryptoPP::StringSink(signature)) { }
			};
			
			SignStream::SignStream(RandomPool& pool, const PrivateKey& privateKey)
				: impl_(new SignStreamImpl(pool, privateKey)) { }
			
			SignStream::~SignStream() { }
			
			bool SignStream::isValid() const {
				return true;
			}
			
			Event::Source SignStream::eventSource() const {
				return Event::Source();
			}
			
			size_t SignStream::write(const uint8_t* data, size_t size) {
				// Apparently this always returns 0,
				// which means success...
				(void) impl_->filter.Put((const byte*) data, size);
				return size;
			}
			
			Buffer SignStream::signature() {
				impl_->filter.MessageEnd();
				
				return Buffer(impl_->signature.begin(), impl_->signature.end());
			}
			
		}
		
	}
	
}

