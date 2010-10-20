#include "../../src/Core.hpp"
#include "Test.hpp"
#include <iostream>

using namespace openp2p::Core;

int main(){
	Wrapper<TestHandler> handler(new TestHandler(1, UDPSocketBuilder(45556)));

	char s[1];
	std::cin.getline(s, 1);

	return 0;
}

