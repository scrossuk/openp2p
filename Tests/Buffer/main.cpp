#include <string>
#include <iostream>

#include "../../src/Core.hpp"

using namespace openp2p::Core;

void TestDataReader(DataReader& reader){
	for(unsigned int i = 0; i < 10; i++){
		Uint8 v;
		try{
			std::cout << "Try read" << std::endl;
			reader >> v;
		}catch(BufferOutOfBoundsException){
			std::cout << "Out of bounds exception" << std::endl;
		}
	}
}

int main(int argc, char *argv[]){
	BufferQueue queue;

	queue.Add(Buffer("Test 1"));
	queue.Add(Buffer(Uint32(42)));
	queue.Add(Buffer("Test 2"));

	DataReader reader(queue);

	std::string test1;
	Uint32 val;
	std::string test2;

	std::cout << "Size = " << queue.Size() << std::endl;

	reader >> test1 >> val >> test2;

	std::cout << "Data = " << test1 << ", " << val << ", " << test2 << std::endl;

	TestDataReader(reader);

	Buffer buffer("A magic string");
	BufferIterator boundsIterator(buffer);

	std::cout << "Buffer Size = " << buffer.Size() << std::endl;

	for(unsigned int i = 0; i < 30; i++){
		unsigned int s = (i * 7) % 30;

		std::cout << "Seek " << s << " => ";
		bool t = false;

		try{
			boundsIterator.Seek(s);
		}catch(BufferOutOfBoundsException){
			t = true;
		}

		if(t){
			std::cout << "Out of bounds exception thrown";
		}else{
			std::cout << "Ok";
		}

		std::cout << ", Position = " << boundsIterator.GetPosition() << std::endl;
	}

	BufferIterator iterator(buffer);
	DataReader bufferReader(iterator);

	Uint16 len;
	bufferReader >> len;
	std::cout << "Length = " << len << std::endl;

	for(unsigned int i = 2; i < buffer.Size(); i++){
		Uint8 c;
		bufferReader >> c;
		std::cout << c;
	}
	std::cout << std::endl;

	TestDataReader(bufferReader);

	return 0;
}

