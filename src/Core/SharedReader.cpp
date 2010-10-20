#include "SharedReader.hpp"
#include "Buffer.hpp"

namespace openp2p{

	namespace Core{

		SharedReader::SharedReader(StreamHandler& handler) : mHandler(handler){
			//...
		}

		void SharedReader::Data(Buffer buffer){

		}

	}

}


