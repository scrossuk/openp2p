#ifndef SHARED_HPP
#define SHARED_HPP

#include "../src/Core.hpp"
#include <iostream>
#include <string>

using namespace openp2p::Core;

class TestSharedStream: public Stream{
	public:
		TestSharedStream(StreamBuilder& builder, bool isClient){
			mStream = builder.BuildStream(Ptr<Stream>(this));

			std::cout << "Stream opened" << std::endl;

			if(isClient){
				mStream->Data(Buffer("A string"));
				std::cout << "Sent data" << std::endl;
			}
		}

		~TestSharedStream(){
			std::cout << "Stream closed" << std::endl;
		}

		void Data(Buffer buffer){
			BufferIterator iterator(buffer);
			DataReader reader(iterator);

			std::string data;
			reader >> data;

			std::cout << "Received " << data << std::endl;

			std::cout << "Closing stream" << std::endl;

			mStream.Reset();
		}

	private:
		Ptr<Stream> mStream;

};

class TestSharedHandler: public StreamHandler{
	public:
		TestSharedHandler(Ptr<StreamSource> source, bool isClient) : mSource(source){
			mIsClient = isClient;
			std::cout << "Shared handler constructed - " << isClient << std::endl;
		}

		~TestSharedHandler(){
			std::cout << "Shared handler destroyed - " << mIsClient << std::endl;
		}

		void OnStream(StreamBuilder& builder){
			std::cout << "TestSharedHandler::OnStream()" << std::endl;
			new TestSharedStream(builder, mIsClient);
		}

	private:
		Ptr<StreamSource> mSource;

		bool mIsClient;

};

class TestSharedFactory: public StreamHandlerFactory{
	public:
		TestSharedFactory(bool isClient){
			mIsClient = isClient;
			std::cout << "Shared factory constructed - " << isClient << std::endl;
		}

		~TestSharedFactory(){
			std::cout << "Shared factory destroyed - " << mIsClient << std::endl;
		}

		Ptr<StreamHandler> CreateHandler(Ptr<StreamSource> source){
			std::cout << "TestSharedFactory::CreateHandler()" << std::endl;
			return new TestSharedHandler(source, mIsClient);
		}

	private:
		bool mIsClient;

};

class TestHandler: public StreamHandler{
	public:
		TestHandler(Ptr<StreamSource> source) : mSource(source){
			std::cout << "Test handler created" << std::endl;
		}

		~TestHandler(){
			std::cout << "Test handler destroyed" << std::endl;
		}

		void OnStream(StreamBuilder& builder){
			std::cout << "TestHandler::OnStream()" << std::endl;

			TestSharedFactory serverFactory(false);
			SharedEndpoint endpoint(builder);

			TestSharedFactory clientFactory(true);
			endpoint.Connect(clientFactory);
			endpoint.Connect(clientFactory);
		}

	private:
		Ptr<StreamSource> mSource;

};

class TestFactory: public StreamHandlerFactory{
	public:
		TestFactory(){
			std::cout << "Test factory created" << std::endl;
		}

		~TestFactory(){
			std::cout << "Test factory destroyed" << std::endl;
		}

		Ptr<StreamHandler> CreateHandler(Ptr<StreamSource> source){
			std::cout << "TestFactory::CreateHandler()" << std::endl;
			return new TestHandler(source);
		}

};

#endif
