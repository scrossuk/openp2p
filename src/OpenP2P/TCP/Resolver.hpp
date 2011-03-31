#ifndef OPENP2P_TCP_RESOLVER_HPP
#define OPENP2P_TCP_RESOLVER_HPP

#include <list>
#include <string>

#include <boost/asio.hpp>
#include <boost/utility.hpp>

#include <OpenP2P/Cancellable.hpp>
#include <OpenP2P/Mutex.hpp>

#include <OpenP2P/TCP/Endpoint.hpp>

namespace OpenP2P{

	namespace TCP{

		class Resolver: public Cancellable, boost::noncopyable{
			public:
				std::list<Endpoint> resolve(const std::string& host, const std::string& service);

				void cancel();

			private:
				Mutex mutex_;
				boost::asio::ip::tcp::resolver internalResolver_;

		};

	}

}

#endif
