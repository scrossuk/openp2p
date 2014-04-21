#include <stdint.h>
#include <cstddef>

#include <cryptopp/osrng.h>

#include <p2p/Crypt/AutoSeededRandomPool.hpp>
#include <p2p/Crypt/RandomPool.hpp>

namespace p2p {

	namespace Crypt {
	
		void AutoSeededRandomPool::generateBlock(uint8_t* data, size_t size) {
			return randPool_.GenerateBlock((byte*) data, size);
		}
		
		AutoSeededRandomPool::operator CryptoPP::RandomPool& () {
			return randPool_;
		}
		
	}
	
}
