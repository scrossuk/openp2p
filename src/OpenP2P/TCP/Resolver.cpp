#include <string>
#include <vector>

#include <boost/asio.hpp>
#include <boost/optional.hpp>

#include <OpenP2P/IOService.hpp>
#include <OpenP2P/Signal.hpp>
#include <OpenP2P/Timeout.hpp>

#include <OpenP2P/TCP/Endpoint.hpp>
#include <OpenP2P/TCP/Resolver.hpp>

namespace OpenP2P {

	namespace TCP {
	
		namespace {
		
			void resolveCallback(Signal* signal, bool* resolveResult, std::vector<TCP::Endpoint>* endpointList,
								 const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::iterator iterator) {
								 
				if (!ec) {
					for (boost::asio::ip::tcp::resolver::iterator end; iterator != end; ++iterator) {
						const boost::asio::ip::tcp::endpoint& endpointImpl = *iterator;
						
						TCP::Endpoint endpoint;
						endpoint.address = IP::Address::FromImpl(endpointImpl.address());
						endpoint.port = endpointImpl.port();
						endpointList->push_back(endpoint);
					}
				}
				
				*resolveResult = !bool(ec);
				signal->activate();
			}
			
		}
		
		Resolver::Resolver() : internalResolver_(GetIOService()) { }
		
		boost::optional< std::vector<TCP::Endpoint> > Resolver::resolve(const std::string& host, const std::string& service, Timeout timeout) {
			boost::asio::ip::tcp::resolver::query query(host, service);
			
			bool resolveResult = false;
			
			Signal signal;
			
			std::vector<TCP::Endpoint> endpointList;
			
			internalResolver_.async_resolve(query,
											boost::bind(resolveCallback, &signal, &resolveResult, &endpointList, _1, _2));
											
			if (signal.wait(timeout)) {
				return resolveResult ? boost::make_optional(endpointList) : boost::optional< std::vector<TCP::Endpoint> >();
			} else {
				internalResolver_.cancel();
				signal.wait();
				return boost::optional< std::vector<TCP::Endpoint> >();
			}
		}
		
	}
	
}

