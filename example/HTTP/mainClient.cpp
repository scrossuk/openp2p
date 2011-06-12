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

		std::size_t writeSome(const uint8_t * data, std::size_t size){
			std::cout << "Write of size " << size << ": ";
			output(data, size);
			std::cout << std::endl;
			size_ += size;
			return size;
		}

		void cancel(){ }

	private:
		std::size_t size_;

};

int main(int argc, char *argv[]){
	if(argc < 2 || argc > 3){
		std::cout << "client <domain name> <optional path>" << std::endl;
		return 0;
	}

	std::string path = (argc == 3) ? argv[2] : "/";

	std::string domain = argv[1];
	
	//Timeout timeout(5.0);

	std::cout << "Resolving..." << std::endl;
	
	TCP::Resolver resolver;

	std::list<TCP::Endpoint> endpointList = resolver.resolve(domain, "http");

	std::cout << "Connecting..." << std::endl;

	TCP::Stream tcpStream;
	
	tcpStream.connect(endpointList);

	if(tcpStream.isConnected()){
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

