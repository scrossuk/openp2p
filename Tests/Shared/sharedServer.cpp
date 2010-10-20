#include "../../src/Core.hpp"
#include "Shared.hpp"

using namespace openp2p::Core;

int main(){
	IOService io;
	TestFactory factory;
	TCPPort port(io, IPv4, 45556);
	port.Listen(factory);

	io.Wait();

	return 0;
}

