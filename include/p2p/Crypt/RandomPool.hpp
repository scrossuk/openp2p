#ifndef P2P_CRYPT_RANDOMPOOL_HPP
#define P2P_CRYPT_RANDOMPOOL_HPP

#include <stdint.h>
#include <cstddef>

#include <cryptopp/osrng.h>

namespace p2p {

	namespace Crypt {
	
		class RandomPool {
			public:
				virtual void generateBlock(uint8_t* data, size_t size) = 0;
				
				virtual operator CryptoPP::RandomPool& () = 0;
				
		};
		
	}
	
}

#endif
