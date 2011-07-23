#ifndef OPENP2P_STREAM_HPP
#define OPENP2P_STREAM_HPP

#include <cstddef>

#include <OpenP2P/Block.hpp>
#include <OpenP2P/Future.hpp>

namespace OpenP2P{

	class IStream{
		public:
			virtual Future<Block> readSome() = 0;

	};

	class OStream{
		public:
			virtual Future<std::size_t> writeSome(const Block& block) = 0;

	};

	class IOStream: public IStream, public OStream{ };

}

#endif
