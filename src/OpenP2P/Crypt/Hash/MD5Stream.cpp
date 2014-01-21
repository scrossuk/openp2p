#include <stdint.h>

#include <string>

// MD5 is not suitable for security
// purposes, so Crypto++ requires
// this to be defined.
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1

#include <cryptopp/filters.h>
#include <cryptopp/md5.h>

#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/Stream.hpp>

#include <OpenP2P/Crypt/Hash/MD5Stream.hpp>

namespace OpenP2P {

	namespace Crypt {
	
		namespace Hash {
		
			struct MD5StreamImpl {
				std::string digest;
				CryptoPP::Weak::MD5 hash;
				std::unique_ptr<CryptoPP::Filter> filter;
				
				inline MD5StreamImpl()
					: filter(new CryptoPP::HashFilter(hash, new CryptoPP::StringSink(digest))) { }
			};
			
			MD5Stream::MD5Stream() : impl_(new MD5StreamImpl()) { }
			
			MD5Stream::~MD5Stream() { }
			
			bool MD5Stream::isValid() const {
				return true;
			}
			
			Event::Source MD5Stream::eventSource() const {
				return Event::Source();
			}
			
			size_t MD5Stream::write(const uint8_t* data, size_t size) {
				// Apparently this always returns 0,
				// which means success...
				(void) impl_->filter->Put((const byte*) data, size);
				return size;
			}
			
			Buffer MD5Stream::calculateDigest() {
				impl_->filter->MessageEnd();
				
				return Buffer(impl_->digest.begin(), impl_->digest.end());
			}
			
		}
		
	}
	
}

