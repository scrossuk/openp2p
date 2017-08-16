#include <iostream>

#include <p2p/Crypt.hpp>
#include <p2p/Crypt/Hash/SHA256Stream.hpp>
#include <p2p/Transport.hpp>

using namespace p2p::Crypt;

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Usage: %s [text]\n", argv[0]);
		return 1;
	}
	
	Hash::SHA256Stream hashStream;
	p2p::BinaryOStream blockingStream(hashStream);
	
	const std::string text = argv[1];
	
	printf("Input: %s\n", text.c_str());
	
	blockingStream.writeAll((const uint8_t*) text.c_str(), text.size());
	
	const p2p::Buffer digest = hashStream.calculateDigest();
	
	printf("Hash length: %llu bytes\n", (unsigned long long) digest.size());
	
	printf("Hash data: ");
	
	for (const auto v: digest) {
		printf("%02x", (int) v);
	}
	
	printf("\n\n");
	
	return 0;
}

