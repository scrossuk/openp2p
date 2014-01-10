#include <stdint.h>
#include <cstddef>
#include <string>
#include <iostream>
#include <OpenP2P.hpp>
#include <OpenP2P/TCP.hpp>

using namespace OpenP2P;

void output(const uint8_t* data, std::size_t size) {
	for (std::size_t i = 0; i < size; i++) {
		std::cout << (char) data[i];
	}
}

class StdOutStream: public OutputStream {
	public:
		StdOutStream() : size_(0) { }
		
		std::size_t size() {
			return size_;
		}
		
		std::size_t waitForSpace(Timeout) {
			return 1000;
		}
		
		bool write(const uint8_t* data, std::size_t dataSize, Timeout) {
			std::cout << "Write of size " << dataSize << ": ";
			output(data, dataSize);
			std::cout << std::endl;
			size_ += dataSize;
			return dataSize;
		}
		
	private:
		std::size_t size_;
		
};

class TextIOStream {
	public:
		TextIOStream(IOStream& stream) : binaryStream_(stream) { }
		
		TextIOStream& operator<<(const std::string& string) {
			binaryStream_.getOutputStream().write((const uint8_t*) string.c_str(), string.size());
			return *this;
		}
		
		TextIOStream& operator>>(OutputStream& stream) {
			BinaryOStream outStream(stream);
			Binary::MoveData(binaryStream_.getInputStream(), outStream);
			return *this;
		}
		
	private:
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
	
	boost::optional< std::vector<IP::Endpoint> > endpointList = resolver.resolve(domain, "http", Timeout::Seconds(5.0));
	
	if (!endpointList) {
		std::cout << "Failed to resolve" << std::endl;
		return 0;
	}
	
	std::cout << "Connecting..." << std::endl;
	
	TCP::Stream tcpStream;
	
	if (!tcpStream.connect(*endpointList, Timeout::Seconds(5.0))) {
		std::cout << "Failed to connect" << std::endl;
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

