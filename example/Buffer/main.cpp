#include <stdint.h>
#include <stdio.h>

#include <OpenP2P.hpp>
#include <boost/asio.hpp>

using namespace OpenP2P;

int main() {
	int a = -4000, b = 42, c = 1;
	
	printf("Writing (%lld, %llu, %llu) to a buffer.\n",
		   (long long) a,
		   (unsigned long long) b,
		   (unsigned long long) c);
		   
	Buffer buffer;
	BufferBuilder builder(buffer);
	BinaryOStream outputStream(builder);
	
	bool writeSuccess = true;
	writeSuccess &= Binary::WriteInt64(outputStream, a);
	writeSuccess &= Binary::WriteUint32(outputStream, b);
	writeSuccess &= Binary::WriteUint8(outputStream, c);
	
	if (!writeSuccess) {
		printf("Failed to write values.\n");
		return -1;
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
	
	if (!readSuccess) {
		printf("Failed to read values.\n");
		return -1;
	}
	
	printf("Read (%lld, %llu, %llu) from buffer.\n",
		   (long long) d,
		   (unsigned long long) e,
		   (unsigned long long) f);
		   
	return 0;
}

