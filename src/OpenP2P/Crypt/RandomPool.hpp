#ifndef OPENP2P_CRYPT_RANDOMPOOL_HPP
#define OPENP2P_CRYPT_RANDOMPOOL_HPP

#include <stdint.h>
#include <cstddef>

#include <cryptopp/osrng.h>

namespace OpenP2P {

	namespace Crypt {
	
		class RandomPool {
			public:
				virtual void generateBlock(uint8_t* data, size_t size) = 0;
				
				virtual operator CryptoPP::RandomPool& () = 0;
				
		};
		
	}
	
}

#endif
