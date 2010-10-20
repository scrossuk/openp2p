#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/function.hpp>

#include "Types.hpp"
#include "Buffer.hpp"
#include "BufferIterator.hpp"
#include "UDPSocket.hpp"
#include "Dispatcher.hpp"
#include "Endpoint.hpp"
#include "Socket.hpp"
#include "Strand.hpp"

using namespace boost::asio::ip;

namespace openp2p{

	namespace Core{

		namespace{

			class UDPInternalSocket{
				public:
					UDPInternalSocket(unsigned short port, boost::function<void (Endpoint, Buffer)> handler) :
						mSocket(GetGlobalIOService()),
						mHandler(handler),
						mCounter(1){

						mIsActive = true;

						mSocket.open(udp::v4());
						boost::asio::socket_base::reuse_address option(true);
						mSocket.set_option(option);
						mSocket.bind(udp::endpoint(udp::v4(), port));

						DoReceive();
					}

					void Send(Endpoint endpoint, Buffer buffer){
						++mCounter;
						mStrand.Schedule(boost::bind(&UDPInternalSocket::DoSend, this, endpoint, buffer));
					}

					void Destroy(){
						mStrand.Schedule(boost::bind(&UDPInternalSocket::DoDestroy, this));
					}

				private:
					~UDPInternalSocket(){ }

					void DoDestroy(){
						mIsActive = false;
						mSocket.close();
						if(mCounter == 0){
							delete this;
						}else{
							mStrand.Schedule(boost::bind(&UDPInternalSocket::DoDestroy, this));
						}
					}

					void DoSend(Endpoint endpoint, Buffer buffer){
						if(!mIsActive){
							--mCounter;
							return;
						}

						Uint8 * data = new Uint8[buffer.Size()];
						BufferIterator iterator(buffer);
						iterator.Read(data, buffer.Size());

						mSocket.async_send_to(boost::asio::buffer(data, buffer.Size()),
							udp::endpoint(endpoint.Address(), endpoint.Port()),
							mStrand.Wrap(boost::bind(&UDPInternalSocket::HandleSend, this, boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred, data)));
					}

					void HandleSend(const boost::system::error_code& error, std::size_t bytesTransferred, Uint8 * data){
						delete[] data;
						--mCounter;
					}

					void DoReceive(){
						if(!mIsActive){
							--mCounter;
							return;
						}

						unsigned int size = 65536;
						mSocket.async_receive_from(boost::asio::buffer(mReadData, size), mRemoteEndpoint,
							mStrand.Wrap(boost::bind(&UDPInternalSocket::HandleReceive, this, boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred)));
					}

					void HandleReceive(const boost::system::error_code& error, std::size_t bytesTransferred){
						if(!mIsActive){
							--mCounter;
							return;
						}

						if(!error){
							Buffer buffer(mReadData, bytesTransferred);

							address addr = mRemoteEndpoint.address();

							Endpoint remoteEndpoint(addr, mRemoteEndpoint.port());

							mStrand.Schedule(boost::bind(mHandler, remoteEndpoint, buffer));

							DoReceive();
						}else{
							--mCounter;
						}
					}

					Strand mStrand;

					boost::asio::ip::udp::socket mSocket;

					boost::asio::ip::udp::endpoint mRemoteEndpoint;

					boost::function<void (Endpoint, Buffer)> mHandler;

					Uint8 mReadData[65536];

					boost::detail::atomic_count mCounter;

					bool mIsActive;

			};

		}

		UDPSocket::UDPSocket(unsigned short port, boost::function<void (Endpoint, Buffer)> handler){
			mInternalSocket = (void *) new UDPInternalSocket(port, handler);
		}

		UDPSocket::~UDPSocket(){
			((UDPInternalSocket *) mInternalSocket)->Destroy();
		}

		void UDPSocket::Send(Endpoint endpoint, Buffer buffer){
			((UDPInternalSocket *) mInternalSocket)->Send(endpoint, buffer);
		}

	}

}

