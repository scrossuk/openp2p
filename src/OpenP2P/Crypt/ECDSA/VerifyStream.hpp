#ifndef OPENP2P_CRYPT_ECDSA_VERIFYSTREAM_HPP
#define OPENP2P_CRYPT_ECDSA_VERIFYSTREAM_HPP

#include <cstddef>

#include <boost/utility.hpp>

#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/Stream.hpp>

#include <OpenP2P/Crypt/ECDSA/PublicKey.hpp>

namespace OpenP2P{

	namespace Crypt{

		namespace ECDSA{

			class VerifyStream: boost::noncopyable, public OutputStream{
				public:
					inline VerifyStream(const PublicKey& publicKey, const Buffer& signature)
						: verifier_(publicKey), isValid_(false){

						filter_ = new CryptoPP::SignatureVerificationFilter(
								verifier_,
								new CryptoPP::ArraySink((byte *) &isValid_, sizeof(isValid_))
							);
						filter_->Put(&signature[0], signature.size());
					}

					inline ~VerifyStream(){
						delete filter_;
					}
					
					inline std::size_t waitForSpace(Timeout){
						// TODO: need to work this out properly.
						return std::numeric_limits<std::size_t>::max();
					}

					inline bool write(const uint8_t * data, std::size_t size, Timeout){
						return filter_->Put((byte *) data, size) == size;
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
