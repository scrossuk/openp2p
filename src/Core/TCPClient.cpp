#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include "Dispatcher.hpp"
#include "Endpoint.hpp"
#include "Strand.hpp"
#include "TCPClient.hpp"
#include "TCPStream.hpp"

using namespace boost::asio::ip;

namespace openp2p{

	namespace Core{

		namespace{

			class TCPInternalClient{
				public:
					TCPInternalClient(Endpoint endpoint, boost::function<void (StreamBuilder)> handler) : mHandler(handler){
						mSocket = new tcp::socket(GetGlobalIOService());

						mIsActive = true;

						mSocket->async_connect(tcp::endpoint(endpoint.Address(), endpoint.Port()),
							mStrand.Wrap(boost::bind(&TCPInternalClient::HandleConnect, this, boost::asio::placeholders::error)));
					}

					void Destroy(){
						mStrand.Schedule(boost::bind(&TCPInternalClient::DoDestroy, this));
					}

				private:
					void DoDestroy(){
						if(mIsActive){
							mIsActive = false;
							mSocket->close();
						}else{
							delete this;
						}
					}

					void HandleConnect(const boost::system::error_code& error){
						if(!mIsActive){
							delete mSocket;
							delete this;
							return;
						}

						if(!error){
							mStrand.Schedule(boost::bind(mHandler, TCPStreamBuilder(mSocket)));
						}else{
							//TODO: report an error
							delete mSocket;
						}

						mIsActive = false;
					}

					Strand mStrand;

					tcp::socket * mSocket;

					boost::function<void (StreamBuilder)> mHandler;

					bool mIsActive;

			};

		}

		TCPClient::TCPClient(Endpoint endpoint, boost::function<void (StreamBuilder)> handler){
			mInternal = (void *) new TCPInternalClient(endpoint, handler);
		}

		TCPClient::~TCPClient(){
			((TCPInternalClient *) mInternal)->Destroy();
		}

	}

}

