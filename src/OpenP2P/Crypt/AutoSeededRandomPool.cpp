#include <stdint.h>
#include <cstddef>

#include <cryptopp/osrng.h>

#include <OpenP2P/Crypt/AutoSeededRandomPool.hpp>
#include <OpenP2P/Crypt/RandomPool.hpp>

namespace OpenP2P {

	namespace Crypt {
	
		void AutoSeededRandomPool::generateBlock(uint8_t* data, size_t size) {
			return randPool_.GenerateBlock((byte*) data, size);
		}
		
		AutoSeededRandomPool::operator CryptoPP::RandomPool& () {
			return randPool_;
		}
		
	}
	
}
