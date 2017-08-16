#ifndef P2P_TCP_RESOLVER_HPP
#define P2P_TCP_RESOLVER_HPP

#include <memory>
#include <string>
#include <vector>

#include <boost/optional.hpp>

#include <p2p/TCP/TCP_api.hpp>
#include <p2p/TCP/Endpoint.hpp>

namespace p2p {

	namespace TCP {
	
		class TCP_OPENP2P_API Resolver {
			public:
				Resolver();
				~Resolver();
				
				boost::optional< std::vector<TCP::Endpoint> > resolve(const std::string& host, const std::string& service);
				
			private:
				// Non-copyable.
				Resolver(const Resolver&) = delete;
				Resolver& operator=(Resolver) = delete;
				
				std::unique_ptr<struct ResolverImpl> impl_;
				
		};
		
	}
	
}

#endif
