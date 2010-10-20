#include "DataReader.hpp"
#include "PacketReader.hpp"
#include "Buffer.hpp"

namespace openp2p{

	namespace Core{

		PacketReader::PacketReader(Stream * stream) : mStream(stream){
			packetLength = 0;
		}

		PacketReader::~PacketReader(){
			delete mStream;
		}

		void PacketReader::Data(Buffer buffer){
			packetData.Add(buffer);

			while(true){
				if(packetLength == 0){
					if(packetData.Size() >= 4){
						DataReader reader(packetData);
						reader >> packetLength;
					}else{
						return;
					}
				}else{
					if(packetData.Size() >= packetLength){
						mStream->Data(packetData.Read(packetLength));
						packetLength = 0;
					}else{
						return;
					}
				}
			}
		}

	}

}


