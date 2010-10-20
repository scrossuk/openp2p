#ifndef OPENP2P_CORE_PACKETSTREAM_HPP
#define OPENP2P_CORE_PACKETSTREAM_HPP

#include <boost/bind.hpp>
#include "Stream.hpp"
#include "PacketReader.hpp"
#include "PacketWriter.hpp"

namespace openp2p{

	namespace Core{

		inline Stream * buildPacketStream(StreamBuilder builder, Stream * stream){
			return new PacketWriter(builder(new PacketReader(stream)));
		}

		inline StreamBuilder PacketStreamBuilder(StreamBuilder builder){
			return boost::bind(buildPacketStream, builder, _1);
		}

	}

}

#endif
