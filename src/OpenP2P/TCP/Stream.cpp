#include <stdint.h>
#include <cstddef>
#include <cstring>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/utility.hpp>

#include <OpenP2P/Future.hpp>
#include <OpenP2P/IOService.hpp>
#include <OpenP2P/Promise.hpp>

#include <OpenP2P/TCP/Endpoint.hpp>
#include <OpenP2P/TCP/Stream.hpp>

namespace OpenP2P{

	namespace TCP{

		namespace{

			void connectCallback(Promise<bool> connectResult, const boost::system::error_code& ec){
				connectResult.setValue(!bool(ec));
			}
			
			//void multiConnectCallback(Promise<bool> connectResult, std::vector<Endpoint> 

			void readCallback(Promise<Block> readResult, MemBlock * memBlock, const boost::system::error_code& ec, std::size_t len){
				if(ec){
					delete memBlock;
					readResult.setValue(Block());
				}else{
					memBlock->expand(0, len);
					readResult.setValue(Block(memBlock, 0, len));
				}
			}
			
			void writeCallback(Promise<std::size_t> writeResult, Block block, const boost::system::error_code& ec, std::size_t len){
				writeResult.setValue(ec ? 0 : len);
			}

		}

		Stream::Stream() : internalSocket_(GetIOService()){ }

		Future<bool> Stream::connect(const Endpoint& endpoint){
			Promise<bool> connectResult;

			internalSocket_.close();
			internalSocket_.async_connect(endpoint, boost::bind(connectCallback, connectResult, _1));

			return connectResult;
		}

		bool Stream::connect(const std::vector<Endpoint>& endpointList){
			for(std::vector<Endpoint>::const_iterator i = endpointList.begin(); i != endpointList.end(); ++i){
				if(connect(*i).get()){
					return true;
				}
			}
			return false;
		}

		boost::asio::ip::tcp::socket& Stream::getInternal(){
			return internalSocket_;
		}

		Future<std::size_t> Stream::writeSome(const Block& block){
			Promise<std::size_t> writeResult;

			internalSocket_.async_write_some(boost::asio::buffer(block.get(), block.size()),
						boost::bind(writeCallback, writeResult, block, _1, _2));

			return writeResult;
		}

		Future<Block> Stream::readSome(){
			Promise<Block> readResult;
			
			MemBlock * memBlock = new MemBlock();

			internalSocket_.async_read_some(boost::asio::buffer(memBlock->get(), BlockSize),
						boost::bind(readCallback, readResult, memBlock, _1, _2));

			return readResult;
		}
		
		void Stream::cancel(){
			internalSocket_.cancel();
		}

		void Stream::close(){
			internalSocket_.close();
		}

	}

}

