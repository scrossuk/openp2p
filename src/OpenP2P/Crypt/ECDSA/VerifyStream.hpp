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
						Block block;
						while((block = iterator.readSome().get()).size() != 0){
							filter_->Put(block.get(), block.size());
						}
					}

					inline ~VerifyStream(){
						delete filter_;
					}

					inline Future<std::size_t> writeSome(const Block& block){
						return filter_->Put((byte *) block.get(), block.size());
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
