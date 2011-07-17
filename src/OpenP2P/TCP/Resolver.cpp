#include <string>
#include <vector>

#include <boost/asio.hpp>

#include <OpenP2P/Future.hpp>
#include <OpenP2P/IOService.hpp>
#include <OpenP2P/Promise.hpp>

#include <OpenP2P/TCP/Endpoint.hpp>
#include <OpenP2P/TCP/Resolver.hpp>

namespace OpenP2P{

	namespace TCP{

		namespace{

			void resolveCallback(Promise< std::vector<Endpoint> > resolveResult,
					const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::iterator iterator){

				std::vector<Endpoint> endpointList;
				if(!ec){
					for(boost::asio::ip::tcp::resolver::iterator end; iterator != end; ++iterator){
						endpointList.push_back(*iterator);
					}
				}
				resolveResult.setValue(endpointList);
			}

		}
		
		Resolver::Resolver() : internalResolver_(GetIOService()){ }

		Future< std::vector<Endpoint> > Resolver::resolve(const std::string& host, const std::string& service){
			std::vector<Endpoint> endpointList;
			boost::asio::ip::tcp::resolver::query query(host, service);
			
			Promise< std::vector<Endpoint> > resolveResult;

			internalResolver_.async_resolve(query,
				boost::bind(resolveCallback, resolveResult, _1, _2));

			return resolveResult;
		}

		void Resolver::cancel(){
			internalResolver_.cancel();
		}

	}

}

