#ifndef OPENP2P_STREAM_HPP
#define OPENP2P_STREAM_HPP

#include <stdint.h>
#include <cstddef>
#include <OpenP2P/Block.hpp>
#include <OpenP2P/WaitHandler.hpp>

namespace OpenP2P{

	class BasicStream{
		public:
			virtual std::size_t readSome(uint8_t *, std::size_t, WaitHandler = Block) = 0;
			
			virtual std::size_t writeSome(const uint8_t *, std::size_t, WaitHandler = Block) = 0;
			
			bool read(uint8_t *, std::size_t, WaitHandler = Block);

			bool write(const uint8_t *, std::size_t, WaitHandler = Block);

	};
	
	BasicStream& operator<<(BasicStream&, bool);
	BasicStream& operator<<(BasicStream&, uint8_t);
	BasicStream& operator<<(BasicStream&, int8_t);
	BasicStream& operator<<(BasicStream&, uint16_t);
	BasicStream& operator<<(BasicStream&, int16_t);
	BasicStream& operator<<(BasicStream&, uint32_t);
	BasicStream& operator<<(BasicStream&, int32_t);
	BasicStream& operator<<(BasicStream&, uint64_t);
	BasicStream& operator<<(BasicStream&, int64_t);
	BasicStream& operator<<(BasicStream&, const char *);
	BasicStream& operator<<(BasicStream&, const std::string&);

	template <typename T, std::size_t N>
	BasicStream& operator<<(BasicStream& stream, const boost::array<T, N>& array){
		for(std::size_t i = 0; i < N; i++){
			stream << array[i];
		}
		return stream;
	}

	template <class T>
	BasicStream& operator<<(BasicStream& stream, const boost::optional<T>& optional){
		if(optional){
			stream << true << *optional;
		}else{
			stream << false;
		}
		return stream;
	}

}

#endif
