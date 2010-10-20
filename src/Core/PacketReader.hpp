#ifndef OPENP2P_CORE_PACKETREADER_HPP
#define OPENP2P_CORE_PACKETREADER_HPP

#include "Stream.hpp"
#include "Buffer.hpp"
#include "BufferQueue.hpp"

namespace openp2p{

	namespace Core{

		class PacketReader: public Stream{
			public:
				PacketReader(Stream *);

				~PacketReader();

				void Data(Buffer);

			private:
				Stream * mStream;

				BufferQueue packetData;

				unsigned int packetLength;

		};

	}

}

#endif
