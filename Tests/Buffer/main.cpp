#include <stdint.h>
#include <cstddef>
#include <string>
#include <iostream>

#include <OpenP2P.hpp>
#include <OpenP2P/TCP.hpp>
#include <boost/asio.hpp>

using namespace OpenP2P;

int main(int argc, char *argv[]){
	BufferBuilder build0;
	BinaryStream stream0(build0);
	stream0 << int64_t(-4000);
	
	BufferBuilder build1;
	BinaryStream stream1(build1);
	stream1 << uint32_t(42);
	
	BufferBuilder build2;
	BinaryStream stream2(build2);
	stream2 << uint8_t(1);

	BufferQueue queue;
	queue.add(build0.getBuffer());
	queue.add(build1.getBuffer());
	queue.add(build2.getBuffer());

	int64_t a;
	uint32_t b;
	uint8_t c;

	BinaryStream queueStream(queue);
	queueStream >> a >> b >> c;

	//All data in queue should have been consumed - so this should fail
	uint8_t someData;
	if(queueStream >> someData){
		std::cout << "Test failed (" << __LINE__ << ") - Queue read gives more data than it should" << std::endl;
		return 1;
	}
	
	BufferBuilder builder;
	BinaryStream builderStream(builder);
	if(!(builderStream << a << b << c)){
		std::cout << "Test failed (" << __LINE__ << ") - Failed to build buffer" << std::endl;
		return 1;
	}
	
	int64_t d;
	uint32_t e;
	uint8_t f;
	
	BufferIterator iterator(builder.getBuffer());
	BinaryStream iteratorStream(iterator);
	if(!(iteratorStream >> d >> e >> f)){
		std::cout << "Test failed (" << __LINE__ << ") - Failed to read from iterator" << std::endl;
		return 1;
	}

	if(a != d || b != e || c != f){
		std::cout << "Test failed (" << __LINE__ << ") - Data from iterator does not match data from building the buffer" << std::endl;
		return 1;
	}
	
	boost::array<TCP::Endpoint, 4> array;
	for(std::size_t i = 0; i < array.size(); i++){
		array[i] = TCP::Endpoint(boost::asio::ip::address_v4::loopback(), i * 4);
	}
	
	boost::array<TCP::Endpoint, 4> array2;
	
	BufferBuilder b2;
	BinaryStream streamb2(b2);
	if(!(streamb2 << array)){
		std::cout << "Test failed (" << __LINE__ << ") - Failed to write to a boost::array" << std::endl;
		return 1;
	}

	BufferIterator it2(b2.getBuffer());
	BinaryStream streamit2(it2);
	if(!(streamit2 >> array2)){
		std::cout << "Test failed (" << __LINE__ << ") - Failed to read from a boost::array" << std::endl;
		return 1;
	}
	
	for(std::size_t i = 0; i < array.size(); i++){
		if(array[i] != array2[i]){
			std::cout << "Test failed (" << __LINE__ << ") - Arrays are not equal" << std::endl;
			return 1;
		}
	}

	std::cout << "Test passed" << std::endl;

	return 0;
}

