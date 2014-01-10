#ifndef OPENP2P_CRYPT_ECDSA_SIGNSTREAM_HPP
#define OPENP2P_CRYPT_ECDSA_SIGNSTREAM_HPP

#include <cstddef>
#include <string>

#include <boost/utility.hpp>

#include <cryptopp/eccrypto.h>
#include <cryptopp/ecp.h>
#include <cryptopp/filters.h>
#include <cryptopp/sha.h>

#include <OpenP2P/BinaryStream.hpp>
#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/BufferBuilder.hpp>
#include <OpenP2P/Stream.hpp>
#include <OpenP2P/StringStream.hpp>

#include <OpenP2P/Crypt/RandomPool.hpp>
#include <OpenP2P/Crypt/ECDSA/PrivateKey.hpp>

namespace OpenP2P {

	namespace Crypt {
	
		namespace ECDSA {
		
			class SignStream: boost::noncopyable, public OutputStream {
				public:
					inline SignStream(RandomPool& pool, const PrivateKey& privateKey)
						: signer_(privateKey) {
						
						filter_ = new CryptoPP::SignerFilter(pool,
															 signer_,
															 new CryptoPP::StringSink(signature_)
															);
					}
					
					inline ~SignStream() {
						delete filter_;
					}
					
					inline EventHandle writeEvent() {
						return EventHandle::True;
					}
					
					inline std::size_t waitForSpace(Timeout) {
						// TODO: need to work this out properly.
						return std::numeric_limits<std::size_t>::max();
					}
					
					inline bool write(const uint8_t* data, std::size_t size, Timeout) {
						return filter_->Put((byte*) data, size) == size;
					}
					
					inline Buffer signature() {
						filter_->MessageEnd();
						
						return Buffer(signature_.begin(), signature_.end());
					}
					
				private:
					std::string signature_;
					CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::Signer signer_;
					CryptoPP::Filter* filter_;
					
			};
			
		}
		
	}
	
}

#endif
