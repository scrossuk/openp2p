#ifndef OPENP2P_CRYPT_ECDSA_VERIFYSTREAM_HPP
#define OPENP2P_CRYPT_ECDSA_VERIFYSTREAM_HPP

#include <cstddef>

#include <boost/utility.hpp>

#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/BufferIterator.hpp>
#include <OpenP2P/Stream.hpp>

#include <OpenP2P/Crypt/ECDSA/PublicKey.hpp>

namespace OpenP2P{

	namespace Crypt{

		namespace ECDSA{

			class VerifyStream: boost::noncopyable, public OpenP2P::OStream{
				public:
					inline VerifyStream(const PublicKey& publicKey, const Buffer& signature)
						: verifier_(publicKey), isValid_(false){

						filter_ = new CryptoPP::SignatureVerificationFilter(
								verifier_,
								new CryptoPP::ArraySink((byte *) &isValid_, sizeof(isValid_))
							);
						BufferIterator iterator(signature);
						const std::size_t dataSize = 1024;
						uint8_t data[dataSize];
						std::size_t readSize;
						while((readSize = iterator.readSome(data, dataSize)) != 0){
							filter_->Put(data, readSize);
						}
					}

					inline ~VerifyStream(){
						delete filter_;
					}
					
					inline EventHandle writeEvent(){
						return EventHandle::True;
					}

					inline std::size_t writeSome(const uint8_t * data, std::size_t dataSize){
						return filter_->Put((byte *) data, dataSize);
					}

					inline bool isValid(){
						filter_->MessageEnd();
						return isValid_;
					}

				private:
					CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::Verifier verifier_;
					CryptoPP::Filter * filter_;
					bool isValid_;
					
			};

		}

	}

}

#endif
