#ifndef OPENP2P_TCP_RESOLVER_HPP
#define OPENP2P_TCP_RESOLVER_HPP

#include <string>
#include <vector>

#include <boost/asio.hpp>
#include <boost/utility.hpp>

#include <OpenP2P/Future.hpp>
#include <OpenP2P/IOService.hpp>
#include <OpenP2P/Mutex.hpp>

#include <OpenP2P/TCP/Endpoint.hpp>

namespace OpenP2P{

	namespace TCP{

		class Resolver: boost::noncopyable{
			public:
				Resolver();
			
				Future< std::vector<Endpoint> > resolve(const std::string& host, const std::string& service);

				void cancel();

			private:
				Mutex mutex_;
				IOService service_;
				boost::asio::ip::tcp::resolver internalResolver_;

		};

	}

}

#endif
