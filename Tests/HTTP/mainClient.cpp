#include <string>
#include <iostream>

#include "../../src/Core.hpp"

using namespace openp2p::Core;

class TestHTTPStream: public Stream{
	public:
		TestHTTPStream(const char * domain, StreamBuilder& builder){
			mStream = builder.BuildStream(Ptr<Stream>(this));

			std::cout << "Socket opened" << std::endl;

			std::string requestString("GET / HTTP/1.0\nHost: ");

			requestString += std::string(domain);

			requestString += std::string("\n\n");

			Buffer request((const Uint8 *) requestString.c_str(), requestString.size());

			mStream->Data(request);

			std::cout << "HTTP request sent" << std::endl;
		}

		~TestHTTPStream(){
			std::cout << std::endl << "Socket closed" << std::endl;
		}

		void Data(Buffer buffer){
			unsigned int size = buffer.Size();

			BufferIterator iterator(buffer);

			Uint8 * data = new Uint8[size + 1];
			iterator.Read(data, size);
			data[size] = 0;

			std::cout << data;

			delete[] data;
		}

	private:
		Ptr<Stream> mStream;

};

class TestHandler: public StreamHandler{
	public:
		TestHandler(const char * domain, Ptr<StreamSource> source) : mSource(source){
			std::cout << "Stream handler created" << std::endl;
			mDomain = domain;
		}

		~TestHandler(){
			std::cout << "Stream handler destroyed" << std::endl;
		}

		void OnStream(StreamBuilder& builder){
			new TestHTTPStream(mDomain, builder);
		}

	private:
		Ptr<StreamSource> mSource;

		const char * mDomain;

};

class TestFactory: public StreamHandlerFactory{
	public:
		TestFactory(const char * domain){
			mDomain = domain;
		}

		Ptr<StreamHandler> CreateHandler(Ptr<StreamSource> source){
			return new TestHandler(mDomain, source);
		}

	private:
		const char * mDomain;

};

int main(int argc, char *argv[]){
	if(argc < 2){
		std::cout << "Needs a domain name..." << std::endl;
		return 0;
	}

	const char * domain = argv[1];

	IOService io;
	TestFactory factory(domain);
	TCPEndpoint endpoint(io, domain, 80);
	endpoint.Connect(factory);

	io.Wait();

	return 0;
}

