#ifndef P2P_CRYPT_ECDSA_VERIFYSTREAM_HPP
#define P2P_CRYPT_ECDSA_VERIFYSTREAM_HPP

#include <stdint.h>

#include <memory>

#include <p2p/Crypt/Crypt_api.hpp>
#include <p2p/Transport.hpp>
#include <p2p/Transport/Stream.hpp>

namespace p2p {

	namespace Crypt {
	
		namespace ECDSA {
		
			class PublicKey;
			
			class CRYPT_OPENP2P_API VerifyStream: public OStream {
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
