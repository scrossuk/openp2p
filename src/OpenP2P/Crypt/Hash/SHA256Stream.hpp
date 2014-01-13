#ifndef OPENP2P_CRYPT_HASH_SHA256STREAM_HPP
#define OPENP2P_CRYPT_HASH_SHA256STREAM_HPP

#include <stdint.h>

#include <memory>

#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/Stream.hpp>

namespace OpenP2P {

	namespace Crypt {
	
		namespace Hash {
		
			class SHA256Stream: public OStream {
				public:
					SHA256Stream();
					~SHA256Stream();
					
					bool isValid() const;
					
					Event::Source eventSource() const;
					
					size_t write(const uint8_t* data, size_t size);
					
					Buffer calculateDigest();
					
				private:
					// Non-copyable.
					SHA256Stream(const SHA256Stream&) = delete;
					SHA256Stream& operator=(SHA256Stream) = delete;
					
					std::unique_ptr<struct SHA256StreamImpl> impl_;
					
			};
			
		}
		
	}
	
}

#endif
