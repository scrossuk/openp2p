#ifndef OPENP2P_CRYPT_ECDSA_VERIFYSTREAM_HPP
#define OPENP2P_CRYPT_ECDSA_VERIFYSTREAM_HPP

#include <cstddef>

#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/BufferIterator.hpp>
#include <OpenP2P/Stream.hpp>

#include <OpenP2P/Crypt/ECDSA/PublicKey.hpp>

namespace OpenP2P{

	namespace Crypt{

		namespace ECDSA{

			class VerifyStream: public OpenP2P::OStream{
				public:
					inline VerifyStream(const PublicKey& publicKey, const Buffer& signature)
						: verifier_(publicKey), isValid_(false){

						filter_ = new CryptoPP::SignatureVerificationFilter(
								verifier_,
								new CryptoPP::ArraySink((byte *) &isValid_, sizeof(isValid_))
							);
						BufferIterator iterator(signature);
						byte data[1024];
						std::size_t size;
						while(size = iterator.readSome((uint8_t *) data, 1024).get()){
							filter_->Put(data, size);
						}
					}

					inline ~VerifyStream(){
						delete filter_;
					}

					inline Future<std::size_t> writeSome(const uint8_t * data, std::size_t size){
						return filter_->Put((byte *) data, size);
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
