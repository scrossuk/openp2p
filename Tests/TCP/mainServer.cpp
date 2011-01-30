#include <stdint.h>
#include <iostream>
#include <boost/thread.hpp>
#include <openp2p.hpp>

void handleStream(openp2p::tcp::stream stream){
	std::cout << "---Started transfer" << std::endl;

	for(unsigned int i = 0; i < 1000; i += 2){
		uint32_t v;
		stream >> v;

		if(v != i){
			std::cout << "Wrong number: " << v << ", Expected: " << (i + 1) << " - Terminating connection" << std::endl;
			return;
		}

		std::cout << "Received: " << i << std::endl;

		stream << uint32_t(i + 1);
	}

	std::cout << "---Successfully completed transfer" << std::endl;
}

int main(){
	while(true){
		std::cout << "Start accept" << std::endl;
		boost::optional<openp2p::tcp::stream> stream = openp2p::tcp::accept(45556);
		std::cout << "Accept returned" << std::endl;
		if(stream){
			//thread per connection
			boost::thread thread(handleStream, *stream);
		}else{
			std::cout << "Error: Accept failed" << std::endl;
			return 1;
		}
	}

	return 0;
}

