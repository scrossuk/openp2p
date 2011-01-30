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

class TestStream: public OpenP2P::IOStream{
	public:
		virtual std::size_t readSome(uint8_t * data, std::size_t size, OpenP2P::WaitHandler handler = OpenP2P::Block){
			return 0;
		}

		virtual std::size_t writeSome(const uint8_t * data, std::size_t size, OpenP2P::WaitHandler handler = OpenP2P::Block){
			std::cout << "Write of size " << size << ": ";
			output(data, size);
			std::cout << std::endl;
			return size;
		}

};

int main(int argc, char *argv[]){
	if(argc < 2 || argc > 3){
		std::cout << "client <domain name> <optional path>" << std::endl;
		return 0;
	}

	std::string path = (argc == 3) ? argv[2] : "/";

	std::string domain = argv[1];

	std::cout << "Resolving..." << std::endl;

	std::list<OpenP2P::TCP::Endpoint> endpointList = OpenP2P::TCP::Resolve(domain, "http", OpenP2P::Timeout(5.0));

	std::cout << "Connecting..." << std::endl;

	boost::optional<OpenP2P::TCP::Stream> opStream = OpenP2P::TCP::Connect(endpointList, OpenP2P::Timeout(10.0));

	if(opStream){
		OpenP2P::TCP::Stream& stream = *opStream;

		std::cout << "Connected" << std::endl;

		stream << "GET " << path << " HTTP/1.1\n";
		stream << "Host: " << domain << "\n";
		stream << "Connection: close\n\n";

		std::cout << "HTTP request sent" << std::endl;

		std::cout << "Receiving data..." << std::endl << std::endl;

		std::size_t size = 0;

		uint8_t data[2048];
		std::size_t s = 0;
		while((s = stream.readSome(data, 2048)) != 0){
			size += s;
			output(data, s);
		}

		std::cout << std::endl << std::endl << "Received: " << size << std::endl;
		std::cout << "Socket closed" << std::endl;
	}else{
		std::cout << "Failed to connect" << std::endl;
	}

	return 0;
}

