#ifndef P2P_CRYPT_ECDSA_SIGNSTREAM_HPP
#define P2P_CRYPT_ECDSA_SIGNSTREAM_HPP

#include <stdint.h>

#include <memory>

#include <p2p/Buffer.hpp>
#include <p2p/Stream.hpp>

namespace p2p {

	namespace Crypt {
	
		class RandomPool;
		
		namespace ECDSA {
		
			class PrivateKey;
			
			class SignStream: public OStream {
				public:
					SignStream(RandomPool& pool, const PrivateKey& privateKey);
					~SignStream();
					
					bool isValid() const;
					
					Event::Source eventSource() const;
					
					size_t write(const uint8_t* data, size_t size);
					
					Buffer signature();
					
				private:
					SignStream(const SignStream&) = delete;
					SignStream& operator=(SignStream) = delete;
					
					std::unique_ptr<struct SignStreamImpl> impl_;
					
			};
			
		}
		
	}
	
}

#endif
