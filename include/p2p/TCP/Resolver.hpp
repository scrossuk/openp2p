#ifndef P2P_TCP_RESOLVER_HPP
#define P2P_TCP_RESOLVER_HPP

#include <string>
#include <vector>

#include <boost/asio.hpp>
#include <boost/optional.hpp>
#include <boost/utility.hpp>

#include <p2p/TCP/Endpoint.hpp>

namespace p2p {

	namespace TCP {
	
		class Resolver: boost::noncopyable {
			public:
				Resolver();
				
				boost::optional< std::vector<TCP::Endpoint> > resolve(const std::string& host, const std::string& service);
				
			private:
				boost::asio::ip::tcp::resolver internalResolver_;
				
		};
		
	}
	
}

#endif
