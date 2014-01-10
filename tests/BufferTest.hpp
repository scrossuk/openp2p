#ifndef BUFFERTEST_HPP
#define BUFFERTEST_HPP

#include <string>
#include <iostream>
#include "Test.hpp"
#include <OpenP2P.hpp>
#include <OpenP2P/TCP.hpp>
#include <boost/asio.hpp>

class BufferTest: public Test {
	public:
		inline std::string name() {
			return "Buffer Test";
		}
		
		inline bool run(Output& output) {
			using namespace OpenP2P;
			{
				int64_t a = -4000;
				uint32_t b = 42;
				uint8_t c = 1;
				
				Buffer buffer;
				BufferBuilder builder(buffer);
				BinaryOStream stream(builder);
				
				if (!(stream << a << b << c)) {
					output.error("Failed to build buffer");
					return false;
				}
				
				int64_t d;
				uint32_t e;
				uint8_t f;
				
				BufferIterator iterator(buffer);
				BinaryIStream readStream(iterator);
				readStream >> d >> e >> f;
				
				if (a != d || b != e || c != f) {
					output.error("Data from iterator does not match data from building the buffer");
					return false;
				}
				
				//All data in buffer should have been consumed - so this should fail
				uint8_t someData;
				
				if (readStream >> someData) {
					output.error("Iterator gives more data than it should");
					return false;
				}
			}
			return true;
		}
		
};

#endif
