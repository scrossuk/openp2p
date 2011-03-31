#include <stdint.h>
#include <cstddef>
#include <string>
#include <iostream>

#include <OpenP2P.hpp>
#include <OpenP2P/TCP.hpp>
#include <boost/asio.hpp>

using namespace OpenP2P;

void TestDataReader(BinaryStream& reader){
	for(std::size_t i = 0; i < 10; i++){
		uint8_t v;
		std::cout << "Try read" << std::endl;
		if(!(reader >> v)){
			std::cout << "Read failed" << std::endl;
		}else{
			std::cout << "Read successful: " << (unsigned int) v << std::endl;
		}
	}
}

int main(int argc, char *argv[]){
	std::cout << "Build 0" << std::endl;
	
	BufferBuilder build0;
	BinaryStream stream0(build0);
	stream0 << int64_t(-4000);
	
	std::cout << "Build 1" << std::endl;
	
	BufferBuilder build1;
	BinaryStream stream1(build1);
	stream1 << uint32_t(42);
	
	std::cout << "Build 2" << std::endl;
	
	BufferBuilder build2;
	BinaryStream stream2(build2);
	stream2 << uint8_t(1);
	
	std::cout << build0.size() << ", " << build1.size() << ", " << build2.size() << std::endl;
	
	printBuffer(build0.getBuffer());
	printBuffer(build1.getBuffer());
	printBuffer(build2.getBuffer());

	BufferQueue queue;
	queue.add(build0.getBuffer());
	queue.add(build1.getBuffer());
	queue.add(build2.getBuffer());
	
	std::cout << "Added to queue" << std::endl;

	int64_t a;
	uint32_t b;
	uint8_t c;

	BinaryStream queueStream(queue);
	queueStream >> a >> b >> c;

	std::cout << "Data = " << a << ", " << b << ", " << (unsigned int) c << std::endl;

	TestDataReader(queue);
	
	BufferBuilder builder;
	BinaryStream builderStream(builder);
	builderStream << a << b << c;
	
	int64_t d;
	uint32_t e;
	uint8_t f;
	
	BufferIterator iterator(*builder);
	BinaryStream iteratorStream(iterator);
	if(iteratorStream >> d >> e >> f){
		std::cout << "Data = " << d << ", " << e << ", " << (unsigned int) f << std::endl;
	}else{
		std::cout << "Couldn't read data" << std::endl;
	}
	
	boost::array<TCP::Endpoint, 4> array;
	for(std::size_t i = 0; i < array.size(); i++){
		array[i] = TCP::Endpoint(boost::asio::ip::address_v4::loopback(), i * 4);
	}
	
	boost::array<TCP::Endpoint, 4> array2;
	
	BufferBuilder b2;
	BinaryStream streamb2(b2);
	if(streamb2 << array){
		std::cout << "Successful array write" << std::endl;
	}
	BufferIterator it2(*b2);
	BinaryStream streamit2(it2);
	if(streamit2 >> array2){
		std::cout << "Successful array read" << std::endl;
	}
	
	for(std::size_t i = 0; i < array.size(); i++){
		std::cout << array[i].port() << std::endl;
	}
	
	for(std::size_t i = 0; i < array2.size(); i++){
		std::cout << array2[i].port() << std::endl;
	}

	return 0;
}

