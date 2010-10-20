#include <iostream>
#include <string>
#include <boost/tuple/tuple.hpp>
#include "../../src/Core.hpp"
#include "../../src/DHT.hpp"

using namespace openp2p::Core;
using namespace openp2p::DHT;

void printBuffer(Buffer buffer){
	try{
		std::string str;
		BufferIterator iterator(buffer);
		DataReader reader(iterator);
		reader >> str;
		
		std::cout << "String: " << str << std::endl;
	}catch(BufferOutOfBoundsException){
		std::cout << "Not a string" << std::endl;
	}
}

class TestDatabase: public Database{
	public:
		TestDatabase(){
			std::cout << "Created database" << std::endl;
			mDatabase.Store(Hash('S'), Buffer("Hello!"));
		}

		~TestDatabase(){
			
		}

		bool Store(Hash key, Buffer data){
			std::cout << "Database store" << std::endl;
			printBuffer(data);
			return mDatabase.Store(key, data);
		}

		boost::tuple<bool, Buffer> Retrieve(Hash key){
			std::cout << "Database retrieve" << std::endl;
			return mDatabase.Retrieve(key);
		}

	private:
		MapDatabase mDatabase;

};

int main(){
	Network network(UDPSocketBuilder(46668), Hash('B'), new TestDatabase());

	char s[1];

	std::cin.getline(s, 1);

	return 0;
}
