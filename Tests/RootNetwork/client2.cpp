#include <stdint.h>
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/optional.hpp>
#include <OpenP2P.hpp>
#include <OpenP2P/RootNetwork.hpp>
#include <OpenP2P/UDP.hpp>

using namespace OpenP2P;

int main(){
	UDP::Socket socket(46668);
	
	RootNetwork::Id id = RootNetwork::Id::Max();
	
	RootNetwork::DHT dht(socket, id);

	std::string str;
	std::getline(std::cin, str);

	return 0;
}

