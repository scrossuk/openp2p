#ifndef TEST_HPP
#define TEST_HPP

#include "../../src/Core.hpp"
#include <iostream>
#include <set>
#include <boost/bind.hpp>

using namespace openp2p::Core;

class TestHandler: public SharedCount{
	public:
		TestHandler(unsigned int, SocketBuilder);
		
		void Destroy();
		
		void OnData(Endpoint, Buffer);
		
	private:
		~TestHandler();
		
		void DoDestroy();
	
		Guard guard;
		unsigned int mV;
		SharedPtr<Socket> mSocket;
	
};

TestHandler::TestHandler(unsigned int v, SocketBuilder builder) : mSocket(builder(guard.Wrap(boost::bind(&TestHandler::OnData, this, _1, _2)))){
	mV = v;
	
	if(v == 1){
		mSocket->Send(Endpoint(boost::asio::ip::address_v4::loopback(), 45557), Buffer(Uint16(0)));
	}
	
	std::cout << "UDP handler created" << std::endl;
}

void TestHandler::Destroy(){
	guard.Schedule(boost::bind(&TestHandler::DoDestroy, this));
	std::cout << "Destroy" << std::endl;
}

TestHandler::~TestHandler(){
	std::cout << "UDP handler destroyed" << std::endl;
}

void TestHandler::DoDestroy(){
	delete this;
}

void TestHandler::OnData(Endpoint endpoint, Buffer data){
	std::cout << "OnData()" << std::endl;
	BufferIterator iterator(data);
	DataReader reader(iterator);
	Uint16 n;
	reader >> n;

	if(n != mV){
		std::cout << "Incorrect data received: " << n << std::endl;
		return;
	}

	std::cout << "Data Received: " << n << std::endl;

	if(n < 10000){
		mV += 2;
		mSocket->Send(endpoint, Buffer(Uint16(n + 1)));
	}

	if(n >= 9999){
		std::cout << "Completed Successfully" << std::endl;
		mV = 0;
	}
}

#endif
