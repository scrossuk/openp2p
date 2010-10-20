#ifndef OPENP2P_CORE_PACKETWRITER_HPP
#define OPENP2P_CORE_PACKETWRITER_HPP

#include "Stream.hpp"

namespace openp2p{

	namespace Core{

		class Buffer;

		class PacketWriter: public Stream{
			public:
				PacketWriter(Stream *);
				~PacketWriter();

				void Data(Buffer);

			private:
				Stream * mStream;

		};

	}

}

#endif
