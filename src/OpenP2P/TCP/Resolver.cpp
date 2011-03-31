#include <list>
#include <string>

#include <boost/asio.hpp>
#include <boost/ref.hpp>

#include <OpenP2P/Condition.hpp>
#include <OpenP2P/Lock.hpp>
#include <OpenP2P/Mutex.hpp>

#include <OpenP2P/TCP/Endpoint.hpp>
#include <OpenP2P/TCP/Resolver.hpp>

namespace OpenP2P{

	namespace TCP{

		namespace{

			void resolveCallback(Mutex& mutex, Condition& condition, std::list<Endpoint>& endpointList,
					const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::iterator iterator){

				Lock lock(mutex);
				if(!ec){
					for(boost::asio::ip::tcp::resolver::iterator end; iterator != end; ++iterator){
						endpointList.push_back(*iterator);
					}
				}
				condition.notify();
			}

		}

		std::list<Endpoint> Resolver::resolve(const std::string& host, const std::string& service){
			Condition condition;
			std::list<Endpoint> endpointList;
			boost::asio::ip::tcp::resolver::query query(host, service);

			Lock lock(mutex_);
			internalResolver_.async_resolve(query,
				boost::bind(resolveCallback, boost::ref(mutex_), boost::ref(condition), boost::ref(endpointList), _1, _2));

			condition.wait(lock);

			return endpointList;
		}

		void Resolver::cancel(){
			Lock lock(mutex_);
			internalResolver_.cancel();
		}

	}

}

