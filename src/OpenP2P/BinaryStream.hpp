#ifndef OPENP2P_BINARYSTREAM_HPP
#define OPENP2P_BINARYSTREAM_HPP

#include <boost/optional.hpp>
#include <OpenP2P/BasicStream.hpp>

namespace OpenP2P{

	class BinaryStream{
		public:
			BinaryStream(BasicStream&);
	};
	
	BinaryStream& operator<<(BinaryStream&, bool);
	BinaryStream& operator<<(BinaryStream&, uint8_t);
	BinaryStream& operator<<(BinaryStream&, int8_t);
	BinaryStream& operator<<(BinaryStream&, uint16_t);
	BinaryStream& operator<<(BinaryStream&, int16_t);
	BinaryStream& operator<<(BinaryStream&, uint32_t);
	BinaryStream& operator<<(BinaryStream&, int32_t);
	BinaryStream& operator<<(BinaryStream&, uint64_t);
	BinaryStream& operator<<(BinaryStream&, int64_t);
	BinaryStream& operator<<(BinaryStream&, const char *);
	BinaryStream& operator<<(BinaryStream&, const std::string&);
	

	template <typename T, std::size_t N>
	BinaryStream& operator<<(BinaryStream& stream, const boost::array<T, N>& array){
		for(std::size_t i = 0; i < N; i++){
			stream << array[i];
		}
		return stream;
	}

	template <class T>
	BinaryStream& operator<<(BinaryStream& stream, const boost::optional<T>& optional){
		if(optional){
			stream << true << *optional;
		}else{
			stream << false;
		}
		return stream;
	}
	
	BinaryStream& operator>>(BinaryStream&, bool&);
	BinaryStream& operator>>(BinaryStream&, uint8_t&);
	BinaryStream& operator>>(BinaryStream&, int8_t&);
	BinaryStream& operator>>(BinaryStream&, uint16_t&);
	BinaryStream& operator>>(BinaryStream&, int16_t&);
	BinaryStream& operator>>(BinaryStream&, uint32_t&);
	BinaryStream& operator>>(BinaryStream&, int32_t&);
	BinaryStream& operator>>(BinaryStream&, uint64_t&);
	BinaryStream& operator>>(BinaryStream&, int64_t&);
	BinaryStream& operator>>(BinaryStream&, std::string&);

	template <typename T, std::size_t N>
	BinaryStream& operator>>(BinaryStream& stream, boost::array<T, N>& array){
		for(std::size_t i = 0; i < N; i++){
			stream >> array[i];
		}
		return stream;
	}

	template <typename T>
	BinaryStream& operator>>(BinaryStream& stream, boost::optional<T>& optional){
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
