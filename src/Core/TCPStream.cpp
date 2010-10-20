#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include "Types.hpp"
#include "Buffer.hpp"
#include "BufferQueue.hpp"
#include "Stream.hpp"
#include "TCPStream.hpp"
#include "Strand.hpp"

using namespace boost::asio::ip;

namespace openp2p{

	namespace Core{

		namespace{

			class TCPInternalStream{
				public:
					TCPInternalStream(tcp::socket * socket, Stream * stream) :
						mStream(stream), mCounter(1){

						mSocket = socket;
						mIsActive = true;
						mIsWriting = false;

						mStrand.Schedule(boost::bind(&TCPInternalStream::DoRead, this));
					}

					void Data(Buffer buffer){
						++mCounter;
						mStrand.Schedule(boost::bind(&TCPInternalStream::DoWrite, this, buffer));
					}

					void Destroy(){
						mStrand.Schedule(boost::bind(&TCPInternalStream::DoDestroy, this));
					}

				private:
					~TCPInternalStream(){
						delete mSocket;
						delete mStream;
					}

					void DoDestroy(){
						DoClose();
						if(mCounter == 0){
						    delete this;
						}else{
						    mStrand.Schedule(boost::bind(&TCPInternalStream::DoDestroy, this));
						}
					}

					void DoClose(){
					    if(mSocket->is_open()){
							mSocket->close();
						}
						mIsActive = false;
					}

					void DoRead(){
						if(!mIsActive){
							--mCounter;
							return;
						}

						unsigned int size = 1024;
						mSocket->async_read_some(boost::asio::buffer(mReadData, size),
							mStrand.Wrap(boost::bind(&TCPInternalStream::HandleRead, this, boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred)));
					}

					void HandleRead(const boost::system::error_code& error, std::size_t bytesTransferred){
						if(!mIsActive){
							--mCounter;
							return;
						}

						if(!error){
							Buffer buffer(mReadData, bytesTransferred);

							mStream->Data(buffer);

							DoRead();
						}else{
							--mCounter;
							DoClose();
						}
					}

					void DoWrite(){
						if(!mIsActive){
							--mCounter;
							return;
						}

						if(mWriteQueue.Size() > 0){
							unsigned int writeSize = mWriteQueue.Size();
							unsigned int size = writeSize > 1024 ? 1024 : writeSize;
							mWriteQueue.Read(mWriteData, size);
							boost::asio::async_write(*mSocket,
								boost::asio::buffer(mWriteData, size),
								mStrand.Wrap(boost::bind(&TCPInternalStream::HandleWrite, this, boost::asio::placeholders::error)));
						}else{
							--mCounter;
							mIsWriting = false;
						}
					}

					void HandleWrite(const boost::system::error_code& error){
						if(!mIsActive){
							--mCounter;
							return;
						}

						if(!error){
							DoWrite();
						}else{
							--mCounter;
							DoClose();
						}
					}

					void DoWrite(Buffer buffer){
						mWriteQueue.Add(buffer);

						if(!mIsWriting && mIsActive){
							mIsWriting = true;
							DoWrite();
						}else{
							--mCounter;
						}
					}

					BufferQueue mWriteQueue;

					Uint8 mReadData[1024];
					Uint8 mWriteData[1024];

					tcp::socket * mSocket;

					Stream * mStream;

					Strand mStrand;

					boost::detail::atomic_count mCounter;

					bool mIsWriting, mIsActive;

			};

		}

		TCPStream::TCPStream(tcp::socket * socket, Stream * stream){
			mInternal = (void *) new TCPInternalStream(socket, stream);
		}

		TCPStream::~TCPStream(){
			((TCPInternalStream *) mInternal)->Destroy();
		}

		void TCPStream::Data(Buffer buffer){
			((TCPInternalStream *) mInternal)->Data(buffer);
		}

	}

}

