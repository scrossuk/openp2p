#include <stdint.h>
#include <cstddef>
#include <string>
#include <iostream>

#include <OpenP2P.hpp>
#include <OpenP2P/TCP.hpp>
#include <boost/asio.hpp>

using namespace OpenP2P;

int main(int argc, char* argv[]) {

	{
		int64_t a = -4000;
		uint32_t b = 42;
		uint8_t c = 1;
		
		Buffer buffer;
		BufferBuilder builder(buffer);
		BinaryStream stream(builder);
		
		if(!(stream << a << b << c)) {
			std::cout << "Test failed (" << __LINE__ << ") - Failed to build buffer" << std::endl;
			return 1;
		}
		
		int64_t d;
		uint32_t e;
		uint8_t f;
		
		BufferIterator iterator(buffer);
		BinaryStream readStream(iterator);
		readStream >> d >> e >> f;
		
		if(a != d || b != e || c != f) {
			std::cout << "Test failed (" << __LINE__ << ") - Data from iterator does not match data from building the buffer" << std::endl;
			return 1;
		}
		
		//All data in buffer should have been consumed - so this should fail
		uint8_t someData;
		
		if(readStream >> someData) {
			std::cout << "Test failed (" << __LINE__ << ") - Iterator gives more data than it should" << std::endl;
			return 1;
		}
	}
	
	{
		Buffer buffer;
		BufferBuilder builder(buffer);
		BinaryStream writeStream(builder);
		
		boost::array<TCP::Endpoint, 4> array;
		
		for(std::size_t i = 0; i < array.size(); i++) {
			array[i] = TCP::Endpoint(boost::asio::ip::address_v4::loopback(), i * 4);
		}
		
		if(!(writeStream << array)) {
			std::cout << "Test failed (" << __LINE__ << ") - Failed to write to a boost::array" << std::endl;
			return 1;
		}
		
		BufferIterator iterator(buffer);
		BinaryStream readStream(iterator);
		
		boost::array<TCP::Endpoint, 4> array2;
		
		if(!(readStream >> array2)) {
			std::cout << "Test failed (" << __LINE__ << ") - Failed to read from a boost::array" << std::endl;
			return 1;
		}
		
		for(std::size_t i = 0; i < array.size(); i++) {
			if(array[i] != array2[i]) {
				std::cout << "Test failed (" << __LINE__ << ") - Arrays are not equal" << std::endl;
				return 1;
			}
		}
	}
	
	std::cout << "Test passed" << std::endl;
	
	return 0;
}

