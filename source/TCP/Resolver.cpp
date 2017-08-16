#include <string>
#include <vector>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/optional.hpp>

#include <p2p/Concurrency/Signal.hpp>

#include <p2p/TCP/Endpoint.hpp>
#include <p2p/TCP/Resolver.hpp>
#include <p2p/Event/IOService.hpp>

namespace p2p {

	namespace TCP {
	
		struct ResolverImpl {
			boost::asio::ip::tcp::resolver resolver;
			
			inline ResolverImpl()
				: resolver(Event::GetIOService()) { }
		};
		
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
		
		Resolver::Resolver() : impl_(new ResolverImpl()) { }
		
		Resolver::~Resolver() { }
		
		// TODO: make this non-blocking!
		boost::optional< std::vector<TCP::Endpoint> > Resolver::resolve(const std::string& host, const std::string& service) {
			boost::asio::ip::tcp::resolver::query query(host, service);
			
			bool resolveResult = false;
			
			Signal signal;
			
			std::vector<TCP::Endpoint> endpointList;
			
			impl_->resolver.async_resolve(query, boost::bind(resolveCallback, &signal, &resolveResult, &endpointList, _1, _2));
											
			signal.wait();
			
			return resolveResult ? boost::make_optional(endpointList) : boost::none;
		}
		
	}
	
}

