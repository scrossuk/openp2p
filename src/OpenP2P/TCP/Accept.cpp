#include <list>
#include <string>
#include <boost/asio.hpp>
#include <boost/optional.hpp>
#include <boost/ref.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include <OpenP2P/CancelFunction.hpp>
#include <OpenP2P/Dispatcher.hpp>
#include <OpenP2P/Wait.hpp>
#include <OpenP2P/WaitCallback.hpp>
#include <OpenP2P/WaitHandler.hpp>
#include <OpenP2P/TCP/Accept.hpp>
#include <OpenP2P/TCP/Stream.hpp>

namespace OpenP2P{

	namespace TCP{

		namespace{

			class AcceptOperation: public WaitObject{
				public:
					AcceptOperation(boost::asio::ip::tcp::acceptor& acceptor) :
						acceptor_(acceptor),
						socket_(new boost::asio::ip::tcp::socket(GlobalIOService())),
						success_(false){ }

					operator bool() const{
						return success_;
					}

					boost::shared_ptr<boost::asio::ip::tcp::socket> operator*(){
						return socket_;
					}

					CancelFunction asyncWait(WaitCallback callback){
						acceptor_.async_accept(*socket_,
							boost::bind(&AcceptOperation::callback, this, callback, boost::asio::placeholders::error));
						return boost::bind(&AcceptOperation::cancel, this);
					}

				private:
					void callback(WaitCallback callback, const boost::system::error_code& ec){
						success_ = ! bool(ec);
						callback();
					}

					void cancel(){
						acceptor_.cancel();
					}

					boost::asio::ip::tcp::acceptor& acceptor_;
					boost::shared_ptr<boost::asio::ip::tcp::socket> socket_;
					bool success_;

			};

		}

		boost::optional<Stream> Accept(unsigned short port, WaitHandler handler){
			boost::asio::ip::tcp::acceptor acceptor(GlobalIOService());
			boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port);
			boost::system::error_code ec;

			acceptor.open(endpoint.protocol(), ec);

			if(ec) return boost::optional<Stream>();

			boost::asio::socket_base::reuse_address option(true);
			acceptor.set_option(option, ec);

			if(ec) return boost::optional<Stream>();

			acceptor.bind(endpoint, ec);

			if(ec) return boost::optional<Stream>();

			acceptor.listen(1, ec);

			if(ec) return boost::optional<Stream>();

			AcceptOperation acceptOp(acceptor);

			Wait(acceptOp, handler);

			if(acceptOp){
				return boost::make_optional(Stream(*acceptOp));
			}else{
				return boost::optional<Stream>();
			}
		}

	}

}

