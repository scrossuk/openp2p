#include <stdint.h>
#include <cstddef>
#include <string>
#include <iostream>
#include <OpenP2P.hpp>
#include <OpenP2P/TCP.hpp>
#include <OpenP2P/Stream/BinaryStream.hpp>

using namespace OpenP2P;

void output(const uint8_t* data, size_t size) {
	for (size_t i = 0; i < size; i++) {
		std::cout << (char) data[i];
	}
}

class StdOutStream: public OStream {
	public:
		StdOutStream() : size_(0) { }
		
		size_t size() {
			return size_;
		}
		
		bool isValid() const {
			return true;
		}
		
		Event::Source eventSource() const {
			return Event::Source();
		}
		
		size_t write(const uint8_t* data, size_t dataSize) {
			std::cout << "Write of size " << dataSize << ": ";
			output(data, dataSize);
			std::cout << std::endl;
			size_ += dataSize;
			return dataSize;
		}
		
	private:
		size_t size_;
		
};

class TextIOStream {
	public:
		TextIOStream(IOStream& stream) : stream_(stream), binaryStream_(stream) { }
		
		TextIOStream& operator<<(const std::string& string) {
			binaryStream_.output().writeAll((const uint8_t*) string.c_str(), string.size());
			return *this;
		}
		
		TextIOStream& operator>>(OStream& stream) {
			Binary::MoveData(stream_, stream);
			return *this;
		}
		
	private:
		IOStream& stream_;
		BinaryIOStream binaryStream_;
		
};

int main(int argc, char* argv[]) {
	if (argc < 2 || argc > 3) {
		std::cout << "client <domain name> <optional path>" << std::endl;
		return 0;
	}
	
	std::string path = (argc == 3) ? argv[2] : "/";
	
	std::string domain = argv[1];
	
	std::cout << "Resolving..." << std::endl;
	
	TCP::Resolver resolver;
	
	const auto endpointList = resolver.resolve(domain, "http", Timeout::Seconds(5.0));
	
	if (!endpointList) {
		std::cout << "Failed to resolve." << std::endl;
		return 0;
	}
	
	std::cout << "Connecting..." << std::endl;
	
	TCP::Stream tcpStream;
	
	if (!tcpStream.connect(*endpointList)) {
		std::cout << "Failed to connect." << std::endl;
		return 0;
	}
	
	TextIOStream stream(tcpStream);
	
	std::cout << "Connected" << std::endl;
	
	stream << "GET " << path << " HTTP/1.1\n";
	stream << "Host: " << domain << "\n";
	stream << "Connection: close\n\n";
	
	std::cout << "HTTP request sent" << std::endl;
	
	std::cout << "Receiving data..." << std::endl << std::endl;
	
	StdOutStream outputStream;
	
	stream >> outputStream;
	
	std::cout << std::endl << std::endl << "Received: " << outputStream.size() << std::endl;
	std::cout << "Socket closed" << std::endl;
	
	return 0;
}

