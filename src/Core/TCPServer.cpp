#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Stream.hpp"
#include "TCPServer.hpp"
#include "TCPStream.hpp"
#include "Dispatcher.hpp"
#include "Strand.hpp"

using namespace boost::asio::ip;

namespace openp2p{

	namespace Core{

		namespace{

			class TCPInternalServer{
				public:
					TCPInternalServer(unsigned short port, boost::function<void (StreamBuilder)> handler) :
						mHandler(handler), mAcceptor(GetGlobalIOService()){

						mIsActive = true;

						tcp::endpoint endpoint(tcp::v4(), port);
						mAcceptor.open(endpoint.protocol());
						mAcceptor.set_option(tcp::acceptor::reuse_address(true));
						mAcceptor.bind(endpoint);
						mAcceptor.listen();
						DoAccept();
					}

					void Destroy(){
						mStrand.Schedule(boost::bind(&TCPInternalServer::DoDestroy, this));
					}

				private:
					~TCPInternalServer(){ }

					void DoDestroy(){
						if(mIsActive){
							mIsActive = false;
							mAcceptor.close();
						}else{
							delete this;
						}
					}

					void DoAccept(){
						if(!mIsActive){
							delete this;
							return;
						}

						tcp::socket * socket = new tcp::socket(GetGlobalIOService());

						mAcceptor.async_accept(*socket,
							mStrand.Wrap(boost::bind(&TCPInternalServer::HandleAccept, this, socket, boost::asio::placeholders::error)));
					}

					void HandleAccept(tcp::socket * socket, const boost::system::error_code& error){
						if(!mIsActive){
							delete this;
							return;
						}

						if(!error){
							mStrand.Schedule(boost::bind(mHandler, TCPStreamBuilder(socket)));
							DoAccept();
						}else{
							delete socket;
							mIsActive = false;
						}
					}

					Strand mStrand;

					boost::function<void (StreamBuilder)> mHandler;

					tcp::acceptor mAcceptor;

					bool mIsActive;

			};
		}

		TCPServer::TCPServer(unsigned short port, boost::function<void (StreamBuilder)> handler){
			mInternal = (void *) new TCPInternalServer(port, handler);
		}

		TCPServer::~TCPServer(){
			((TCPInternalServer *) mInternal)->Destroy();
		}

	}

}

