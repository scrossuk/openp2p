#ifndef OPENP2P_CRYPT_ECC_PUBLICKEY_HPP
#define OPENP2P_CRYPT_ECC_PUBLICKEY_HPP

namespace OpenP2P{

	namespace Crypt{

		namespace ECC{

			enum KeySize{
				KEYSIZE_256BITS = 32,
				KEYSIZE_512BITS = 64
			};

			template <KeySize keySize>
			class PublicKey;

		}

	}

}

#endif
