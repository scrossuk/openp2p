#include <stdint.h>
#include <cstddef>
#include <string>
#include <iostream>
#include <OpenP2P.hpp>
#include <OpenP2P/TCP.hpp>

using namespace OpenP2P;

void output(const uint8_t * data, std::size_t size){
	for(std::size_t i = 0; i < size; i++){
		std::cout << (char) data[i];
	}
}

class OutputStream: public OStream{
	public:
		OutputStream() : size_(0){ }

		std::size_t size(){
			return size_;
		}
		
		EventHandle writeEvent(){
			return EventHandle::True;
		}

		std::size_t writeSome(const uint8_t * data, std::size_t dataSize){
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
		TextIOStream(IOStream& stream) : binaryStream_(stream){ }
	
		TextIOStream& operator<<(const std::string& string){
			binaryStream_.write((const uint8_t *) string.c_str(), string.size());
			return *this;
		}
		
		TextIOStream& operator>>(OStream& stream){
			binaryStream_ >> stream;
			return *this;
		}
		
	private:
		BinaryIOStream binaryStream_;
	
};

int main(int argc, char *argv[]){
	if(argc < 2 || argc > 3){
		std::cout << "client <domain name> <optional path>" << std::endl;
		return 0;
	}

	std::string path = (argc == 3) ? argv[2] : "/";

	std::string domain = argv[1];
	
	std::cout << "Resolving..." << std::endl;
	
	TCP::Resolver resolver;

	std::vector<TCP::Endpoint> endpointList = resolver.resolve(domain, "http").get();

	std::cout << "Connecting..." << std::endl;

	TCP::Stream tcpStream;
	
	Future<bool> connect = tcpStream.connect(endpointList);
	
	if(!connect.timedWait(5.0)){
		std::cout << "Timed out" << std::endl;
		return 0;
	}

	if(connect.get()){
		TextIOStream stream(tcpStream);

		std::cout << "Connected" << std::endl;

		stream << "GET " << path << " HTTP/1.1\n";
		stream << "Host: " << domain << "\n";
		stream << "Connection: close\n\n";

		std::cout << "HTTP request sent" << std::endl;

		std::cout << "Receiving data..." << std::endl << std::endl;

		OutputStream outputStream;

		stream >> outputStream;

		std::cout << std::endl << std::endl << "Received: " << outputStream.size() << std::endl;
		std::cout << "Socket closed" << std::endl;
	}else{
		std::cout << "Failed to connect" << std::endl;
	}

	return 0;
}

