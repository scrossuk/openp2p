#ifndef OPENP2P_ISTREAM_HPP
#define OPENP2P_ISTREAM_HPP

#include <stdint.h>
#include <cstddef>
#include <string>
#include <boost/array.hpp>
#include <boost/optional.hpp>
#include <OpenP2P/Stream.hpp>
#include <OpenP2P/Block.hpp>
#include <OpenP2P/WaitHandler.hpp>

namespace OpenP2P{

	class IStream: public virtual Stream{
		public:
			virtual std::size_t readSome(uint8_t *, std::size_t, WaitHandler = Block) = 0;

			bool tryRead(uint8_t *, std::size_t, WaitHandler = Block);

			void read(uint8_t *, std::size_t, WaitHandler = Block);

	};

	IStream& operator>>(IStream&, bool&);
	IStream& operator>>(IStream&, uint8_t&);
	IStream& operator>>(IStream&, int8_t&);
	IStream& operator>>(IStream&, uint16_t&);
	IStream& operator>>(IStream&, int16_t&);
	IStream& operator>>(IStream&, uint32_t&);
	IStream& operator>>(IStream&, int32_t&);
	IStream& operator>>(IStream&, uint64_t&);
	IStream& operator>>(IStream&, int64_t&);
	IStream& operator>>(IStream&, std::string&);

	template <typename T, std::size_t N>
	IStream& operator>>(IStream& stream, boost::array<T, N>& array){
		for(std::size_t i = 0; i < N; i++){
			stream >> array[i];
		}
		return stream;
	}

	template <typename T>
	IStream& operator>>(IStream& stream, boost::optional<T>& optional){
		bool b;
		stream >> b;
		if(b){
			T v;
			stream >> v;
			optional = boost::make_optional(v);
		}else{
			optional = boost::optional<T>();
		}
		return stream;
	}

}

#endif
