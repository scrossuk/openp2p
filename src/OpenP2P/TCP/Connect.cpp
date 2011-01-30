#include <list>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/optional.hpp>
#include <boost/ref.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include <OpenP2P/CancelFunction.hpp>
#include <OpenP2P/Dispatcher.hpp>
#include <OpenP2P/Signal.hpp>
#include <OpenP2P/Wait.hpp>
#include <OpenP2P/WaitCallback.hpp>
#include <OpenP2P/WaitHandler.hpp>
#include <OpenP2P/TCP/Connect.hpp>
#include <OpenP2P/TCP/Endpoint.hpp>
#include <OpenP2P/TCP/Stream.hpp>

namespace OpenP2P{

	namespace TCP{

		namespace{

			class ConnectOperation: public WaitObject{
				public:
					ConnectOperation(const Endpoint& endpoint) :
						endpoint_(endpoint),
						socket_(new boost::asio::ip::tcp::socket(GlobalIOService())),
						success_(false){ }

					operator bool() const{
						return success_;
					}

					boost::shared_ptr<boost::asio::ip::tcp::socket> operator*(){
						return socket_;
					}

					CancelFunction asyncWait(WaitCallback callback){
						socket_->async_connect(endpoint_,
							boost::bind(&ConnectOperation::callback, this, callback, boost::asio::placeholders::error));
						return boost::bind(&ConnectOperation::cancel, this);
					}

					void callback(WaitCallback callback, const boost::system::error_code& ec){
						success_ = ! bool(ec);
						callback();
					}

					void cancel(){
						socket_->cancel();
					}

				private:
					Endpoint endpoint_;
					boost::shared_ptr<boost::asio::ip::tcp::socket> socket_;
					bool success_;

			};

		}

		boost::optional<Stream> Connect(const Endpoint& endpoint, WaitHandler handler){
			ConnectOperation connectOp(endpoint);
			Wait(connectOp, handler);

			if(connectOp){
				return boost::make_optional(Stream(*connectOp));
			}else{
				return boost::optional<Stream>();
			}
		}

		boost::optional<Stream> Connect(const std::list<Endpoint>& endpointList, WaitHandler handler){
			Signal endSignal;
			CancelFunction cancel = handler.asyncWait(boost::bind(&Signal::activate, &endSignal));

			for(std::list<Endpoint>::const_iterator p = endpointList.begin(); p != endpointList.end(); ++p){
				boost::optional<Stream> s = Connect(*p, endSignal);
				if(s){
					cancel();
					Wait(endSignal);
					return s;
				}
			}

			cancel();

			Wait(endSignal);

			return boost::optional<Stream>();
		}

	}

}

