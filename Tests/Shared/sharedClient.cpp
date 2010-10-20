#include "../../src/Core.hpp"
#include "Shared.hpp"

using namespace openp2p::Core;

int main(){
	IOService io;
	TestFactory factory;
	TCPEndpoint endpoint(io, "localhost", 45556);
	endpoint.Connect(factory);

	io.Wait();

	return 0;
}

