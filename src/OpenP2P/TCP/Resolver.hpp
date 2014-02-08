#ifndef OPENP2P_TCP_RESOLVER_HPP
#define OPENP2P_TCP_RESOLVER_HPP

#include <string>
#include <vector>

#include <boost/asio.hpp>
#include <boost/optional.hpp>
#include <boost/utility.hpp>

#include <OpenP2P/Timeout.hpp>

#include <OpenP2P/TCP/Endpoint.hpp>

namespace OpenP2P {

	namespace TCP {
	
		class Resolver: boost::noncopyable {
			public:
				Resolver();
				
				boost::optional< std::vector<TCP::Endpoint> > resolve(const std::string& host, const std::string& service, Timeout timeout = Timeout::Infinite());
				
			private:
				boost::asio::ip::tcp::resolver internalResolver_;
				
		};
		
	}
	
}

#endif
