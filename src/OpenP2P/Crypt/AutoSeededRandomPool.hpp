#ifndef OPENP2P_CRYPT_AUTOSEEDEDRANDOMPOOL_HPP
#define OPENP2P_CRYPT_AUTOSEEDEDRANDOMPOOL_HPP

#include <stdint.h>
#include <cstddef>

#include <cryptopp/osrng.h>

#include <OpenP2P/Crypt/RandomPool.hpp>

namespace OpenP2P {

	namespace Crypt {
	
		class AutoSeededRandomPool: public RandomPool {
			public:
				void generateBlock(uint8_t* data, size_t size);
				
				operator CryptoPP::RandomPool& ();
				
			private:
				CryptoPP::AutoSeededRandomPool randPool_;
				
		};
		
	}
	
}

#endif
