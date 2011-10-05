#include <stdint.h>
#include <iostream>

#include <OpenP2P.hpp>
#include <boost/asio.hpp>

using namespace OpenP2P;

int main(int argc, char* argv[]) {
	int a = -4000, b = 42, c = 1;
	
	std::cout << "Writing (" << a << ", " << b << ", " << c << ") to a buffer" << std::endl;
	
	Buffer buffer;
	BufferBuilder builder(buffer);
	BinaryOStream outputStream(builder);
	
	bool writeSuccess = true;
	writeSuccess &= Binary::WriteInt64(outputStream, a);
	writeSuccess &= Binary::WriteUint32(outputStream, b);
	writeSuccess &= Binary::WriteUint8(outputStream, c);
	
	if(!writeSuccess){
		std::cout << "Failed to write values" << std::endl;
		return 0;
	}
	
	int64_t d;
	uint32_t e;
	uint8_t f;
	
	BufferIterator iterator(buffer);
	BinaryIStream inputStream(iterator);
	
	bool readSuccess = true;
	readSuccess &= Binary::ReadInt64(inputStream, &d);
	readSuccess &= Binary::ReadUint32(inputStream, &e);
	readSuccess &= Binary::ReadUint8(inputStream, &f);
	
	if(!readSuccess){
		std::cout << "Failed to read values" << std::endl;
	}
	
	std::cout << "Read (" << d << ", " << e << ", " << f << ") from buffer" << std::endl;
	
	return 0;
}

