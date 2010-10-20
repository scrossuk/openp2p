#include "../../src/Core.hpp"
#include "Test.hpp"
#include <boost/bind.hpp>
#include <iostream>

using namespace openp2p::Core;

int main(){
	TCPServer server(45556, boost::bind(testHandler, false, _1));

	char s[1];

	std::cin.getline(s, 1);

	return 0;
}

