#ifndef TEST_HPP
#define TEST_HPP

#include "../../src/Core.hpp"
#include <iostream>
#include <set>

using namespace openp2p::Core;

class TestStream: public Stream{
	public:
		TestStream(bool start, StreamBuilder builder) : mStream(builder(this)){
			mStart = start;

			std::cout << "Socket opened" << std::endl;

			nextValue = 0;

			if(mStart){
				std::cout << "Send 0" << std::endl;
				mStream->Data(Buffer(Uint32(0)));
			}
		}
		
		~TestStream(){
			std::cout << "Socket closed" << std::endl;
		}

		void Data(Buffer buffer){
			BufferIterator iterator(buffer);
			DataReader reader(iterator);

			Uint32 v;
			reader >> v;

			if(v != nextValue){
				std::cout << "ERROR!" << std::endl;
				delete mStream;
				return;
			}

			if(mStart && v >= 100){
				std::cout << "End" << std::endl;
				delete mStream;
				return;
			}

			nextValue++;

			std::cout << "Got " << v << " => ";
			if(mStart){
				mStream->Data(Buffer(Uint32(v + 1)));
				std::cout << "Send " << (v + 1) << std::endl;
			}else{
				mStream->Data(Buffer(Uint32(v)));
				std::cout << "Echo" << std::endl;
			}
		}

	private:
		Stream * mStream;

		unsigned int nextValue;

		bool mStart;

};

void testHandler(bool isClient, StreamBuilder builder){
	std::cout << "OnStream()" << std::endl;
	new TestStream(isClient, PacketStreamBuilder(builder));
}

#endif
