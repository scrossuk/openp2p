#include "../../src/Core.hpp"
#include "Test.hpp"
#include <boost/bind.hpp>
#include <iostream>

using namespace openp2p::Core;
using namespace boost::asio::ip;

int main(){
	TCPClient client(Endpoint(address_v4::loopback(), 45556), boost::bind(testHandler, true, _1));
	
	char s[1];

	std::cin.getline(s, 1);

	return 0;
}

