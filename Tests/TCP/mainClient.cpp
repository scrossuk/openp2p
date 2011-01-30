#include <stdint.h>
#include <iostream>
#include <openp2p.hpp>
#include <boost/bind.hpp>
#include <boost/optional.hpp>

void blah(){
	std::cout << "blah" << std::endl;
}

int main(){
	//openp2p::wait(openp2p::timeout(5.0));
	
	std::cout << "Blah" << std::endl;
	boost::optional<openp2p::tcp::stream> stream = openp2p::tcp::connect(openp2p::tcp::endpoint(boost::asio::ip::address_v4::loopback(), 45556), openp2p::timeout(5.0));

	std::cout << "Starting" << std::endl;

	if(stream){
		for(unsigned int i = 0; i < 1000; i += 2){
			*stream << uint32_t(i);

			uint32_t v;
			*stream >> v;

			if(v != (i + 1)){
				std::cout << "Wrong number: " << v << ", Expected: " << (i + 1) << std::endl;
				return 1;
			}
		}
	}else{
		std::cout << "Failed to connect" << std::endl;
	}

	return 0;
}

