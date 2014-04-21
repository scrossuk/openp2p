#ifndef P2P_CRYPT_AES_KEY_HPP
#define P2P_CRYPT_AES_KEY_HPP

namespace p2p {

	namespace Crypt {
	
		namespace AES {
		
			enum KeySize {
				KEYSIZE_128BITS = 16,
				KEYSIZE_192BITS = 24,
				KEYSIZE_256BITS = 32
			};
			
			template <KeySize keySize>
			class Key;
			
		}
		
	}
	
}

#endif
