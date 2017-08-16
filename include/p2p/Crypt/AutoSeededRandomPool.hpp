#ifndef P2P_CRYPT_AUTOSEEDEDRANDOMPOOL_HPP
#define P2P_CRYPT_AUTOSEEDEDRANDOMPOOL_HPP

#include <stdint.h>
#include <cstddef>

#include <cryptopp/osrng.h>

#include <p2p/Crypt/Crypt_api.hpp>
#include <p2p/Crypt/RandomPool.hpp>

namespace p2p {

	namespace Crypt {
	
		class CRYPT_OPENP2P_API AutoSeededRandomPool: public RandomPool {
			public:
				void generateBlock(uint8_t* data, size_t size);
				
				operator CryptoPP::RandomPool& ();
				
			private:
				CryptoPP::AutoSeededRandomPool randPool_;
				
		};
		
	}
	
}

#endif
