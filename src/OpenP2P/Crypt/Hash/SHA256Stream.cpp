#include <stdint.h>

#include <string>

#include <cryptopp/filters.h>
#include <cryptopp/sha.h>

#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/Stream.hpp>

#include <OpenP2P/Crypt/Hash/SHA256Stream.hpp>

namespace OpenP2P {

	namespace Crypt {
	
		namespace Hash {
		
			struct SHA256StreamImpl {
				std::string digest;
				CryptoPP::SHA256 hash;
				std::unique_ptr<CryptoPP::Filter> filter;
				
				inline SHA256StreamImpl()
					: filter(new CryptoPP::HashFilter(hash, new CryptoPP::StringSink(digest))) { }
			};
			
			SHA256Stream::SHA256Stream() : impl_(new SHA256StreamImpl()) { }
			
			SHA256Stream::~SHA256Stream() { }
			
			bool SHA256Stream::isValid() const {
				return true;
			}
			
			Event::Source SHA256Stream::eventSource() const {
				return Event::Source();
			}
			
			size_t SHA256Stream::write(const uint8_t* data, size_t size) {
				// Apparently this always returns 0,
				// which means success...
				(void) impl_->filter->Put((const byte*) data, size);
				return size;
			}
			
			Buffer SHA256Stream::calculateDigest() {
				impl_->filter->MessageEnd();
				
				return Buffer(impl_->digest.begin(), impl_->digest.end());
			}
			
		}
		
	}
	
}

