#ifndef OPENP2P_CRYPT_ECDSA_VERIFYSTREAM_HPP
#define OPENP2P_CRYPT_ECDSA_VERIFYSTREAM_HPP

#include <stdint.h>

#include <memory>

#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/Stream.hpp>

namespace OpenP2P {

	namespace Crypt {
	
		namespace ECDSA {
		
			class PublicKey;
			
			class VerifyStream: public OStream {
				public:
					VerifyStream(const PublicKey& publicKey, const Buffer& signature);
					~VerifyStream();
					
					bool isValid() const;
					
					Event::Source eventSource() const;
					
					size_t write(const uint8_t* data, size_t size);
					
					bool isSignatureValid();
					
				private:
					VerifyStream(const VerifyStream&) = delete;
					VerifyStream& operator=(VerifyStream) = delete;
					
					std::unique_ptr<struct VerifyStreamImpl> impl_;
					
			};
			
		}
		
	}
	
}

#endif
