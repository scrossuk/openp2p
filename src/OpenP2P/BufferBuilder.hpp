#ifndef OPENP2P_BUFFERBUILDER_HPP
#define OPENP2P_BUFFERBUILDER_HPP

#include <stdint.h>
#include <cstddef>
#include <OpenP2P/Block.hpp>
#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/IStream.hpp>
#include <OpenP2P/OStream.hpp>
#include <OpenP2P/WaitHandler.hpp>

namespace OpenP2P{

	class BufferBuilder: public OStream{
		public:
			BufferBuilder();

			std::size_t size();

			std::size_t writeSome(const uint8_t *, std::size_t, WaitHandler = Block);

			Buffer operator*() const;

		private:
			Buffer buffer_;

	};

	template <typename T>
	Buffer MakeBuffer(const T& v){
		BufferBuilder builder;
		builder << v;
		return *builder;
	}

	IStream& operator>>(IStream&, Buffer&);

}

#endif
