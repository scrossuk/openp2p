#include <stdint.h>
#include <cstddef>
#include <string>
#include <iostream>

#include <OpenP2P.hpp>
#include <OpenP2P/TCP.hpp>
#include <boost/asio.hpp>

void TestDataReader(OpenP2P::IStream& reader){
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
	OpenP2P::BufferQueue queue;
	
	std::cout << "Build 0" << std::endl;
	
	OpenP2P::BufferBuilder build0;
	build0 << int64_t(-4000);
	
	std::cout << "Build 1" << std::endl;
	
	OpenP2P::BufferBuilder build1;
	build1 << uint32_t(42);
	
	std::cout << "Build 2" << std::endl;
	
	OpenP2P::BufferBuilder build2;
	build2 << uint8_t(1);
	
	std::cout << build0.size() << ", " << build1.size() << ", " << build2.size() << std::endl;
	
	printBuffer(*build0);
	printBuffer(*build1);
	printBuffer(*build2);

	queue.add(*build0);
	queue.add(*build1);
	queue.add(*build2);
	
	std::cout << "Added to queue" << std::endl;

	int64_t a;
	uint32_t b;
	uint8_t c;

	queue >> a >> b >> c;

	std::cout << "Data = " << a << ", " << b << ", " << (unsigned int) c << std::endl;

	TestDataReader(queue);
	
	OpenP2P::BufferBuilder builder;
	builder << a << b << c;
	
	int64_t d;
	uint32_t e;
	uint8_t f;
	
	OpenP2P::BufferIterator iterator(*builder);
	if(iterator >> d >> e >> f){
		std::cout << "Data = " << d << ", " << e << ", " << (unsigned int) f << std::endl;
	}else{
		std::cout << "Couldn't read data" << std::endl;
	}
	
	boost::array<OpenP2P::TCP::Endpoint, 4> array;
	for(std::size_t i = 0; i < array.size(); i++){
		array[i] = OpenP2P::TCP::Endpoint(boost::asio::ip::address_v4::loopback(), i * 4);
	}
	
	boost::array<OpenP2P::TCP::Endpoint, 4> array2;
	
	OpenP2P::BufferBuilder b2;
	if(b2 << array){
		std::cout << "Successful array write" << std::endl;
	}
	OpenP2P::BufferIterator it2(*b2);
	if(it2 >> array2){
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

