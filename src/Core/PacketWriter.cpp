#include "PacketWriter.hpp"
#include "Buffer.hpp"

namespace openp2p{

	namespace Core{

		PacketWriter::PacketWriter(Stream * stream) : mStream(stream){
			//...
		}
		
		PacketWriter::~PacketWriter(){
			delete mStream;
		}

		void PacketWriter::Data(Buffer data){
			Buffer sizeField(Uint32(data.Size()));
			mStream->Data(sizeField + data);
		}

	}

}


