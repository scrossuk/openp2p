#include <list>
#include <string>
#include <boost/asio.hpp>
#include <boost/ref.hpp>
#include <boost/utility.hpp>

#include <OpenP2P/CancelFunction.hpp>
#include <OpenP2P/Dispatcher.hpp>
#include <OpenP2P/Join.hpp>
#include <OpenP2P/Wait.hpp>
#include <OpenP2P/WaitCallback.hpp>
#include <OpenP2P/WaitHandler.hpp>
#include <OpenP2P/TCP/Endpoint.hpp>
#include <OpenP2P/TCP/Resolve.hpp>

namespace OpenP2P{

	namespace TCP{

		namespace{

			class ResolveOperation: public WaitObject{
				public:
					ResolveOperation(const std::string& host, const std::string& service) :
						query_(host, service), resolver_(GlobalIOService()){ }

					std::list<Endpoint> operator*(){
						return list_;
					}

					CancelFunction asyncWait(WaitCallback callback){
						resolver_.async_resolve(query_,
							boost::bind(&ResolveOperation::callback, this, callback, _1, _2));
						return boost::bind(&ResolveOperation::cancel, this);
					}

					void callback(WaitCallback callback, const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::iterator iterator){
						if(!ec){
							for(boost::asio::ip::tcp::resolver::iterator end; iterator != end; ++iterator){
								list_.push_back(*iterator);
							}
						}

						callback();
					}

					void cancel(){
						resolver_.cancel();
					}

				private:
					boost::asio::ip::tcp::resolver::query query_;
					boost::asio::ip::tcp::resolver resolver_;
					std::list<Endpoint> list_;

			};

		}

		std::list<Endpoint> Resolve(const std::string& host, const std::string& service, WaitHandler handler){
			ResolveOperation resolveOp(host, service);
			Wait(resolveOp, handler);

			return *resolveOp;
		}

	}

}

