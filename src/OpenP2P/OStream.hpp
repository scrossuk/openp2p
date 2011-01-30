#ifndef OPENP2P_BASICOSTREAM_HPP
#define OPENP2P_BASICOSTREAM_HPP

#include <stdint.h>
#include <cstddef>
#include <boost/array.hpp>
#include <boost/optional.hpp>
#include <OpenP2P/Stream.hpp>
#include <OpenP2P/Block.hpp>
#include <OpenP2P/WaitHandler.hpp>

namespace OpenP2P{

	class OStream: public virtual Stream{
		public:
			virtual std::size_t writeSome(const uint8_t *, std::size_t, WaitHandler = Block) = 0;

			bool tryWrite(const uint8_t *, std::size_t, WaitHandler = Block);

			void write(const uint8_t *, std::size_t, WaitHandler = Block);

	};

	OStream& operator<<(OStream&, bool);
	OStream& operator<<(OStream&, uint8_t);
	OStream& operator<<(OStream&, int8_t);
	OStream& operator<<(OStream&, uint16_t);
	OStream& operator<<(OStream&, int16_t);
	OStream& operator<<(OStream&, uint32_t);
	OStream& operator<<(OStream&, int32_t);
	OStream& operator<<(OStream&, uint64_t);
	OStream& operator<<(OStream&, int64_t);
	OStream& operator<<(OStream&, const char *);
	OStream& operator<<(OStream&, const std::string&);

	template <typename T, std::size_t N>
	OStream& operator<<(OStream& stream, const boost::array<T, N>& array){
		for(std::size_t i = 0; i < N; i++){
			stream << array[i];
		}
		return stream;
	}

	template <class T>
	OStream& operator<<(OStream& stream, const boost::optional<T>& optional){
		if(optional){
			stream << true << *optional;
		}else{
			stream << false;
		}
		return stream;
	}

}

#endif
