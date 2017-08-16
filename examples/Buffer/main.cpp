#include <stdint.h>
#include <stdio.h>

#include <p2p/Transport.hpp>

using namespace p2p;

int main() {
	int a = -4000, b = 42, c = 1;
	
	printf("Writing (%lld, %llu, %llu) to a buffer.\n",
		   (long long) a,
		   (unsigned long long) b,
		   (unsigned long long) c);
		   
	Buffer buffer;
	BufferBuilder builder(buffer);
	BinaryOStream outputStream(builder);
	
	Binary::WriteInt64(outputStream, a);
	Binary::WriteUint32(outputStream, b);
	Binary::WriteUint8(outputStream, c);
	
	BufferIterator iterator(buffer);
	BinaryIStream inputStream(iterator);
	
	const int64_t d = Binary::ReadInt64(inputStream);
	const uint32_t e = Binary::ReadUint32(inputStream);
	const uint8_t f = Binary::ReadUint8(inputStream);
	
	printf("Read (%lld, %llu, %llu) from buffer.\n",
		   (long long) d,
		   (unsigned long long) e,
		   (unsigned long long) f);
		   
	return 0;
}

