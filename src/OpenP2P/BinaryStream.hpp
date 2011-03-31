#ifndef OPENP2P_BINARYSTREAM_HPP
#define OPENP2P_BINARYSTREAM_HPP

#include <boost/array.hpp>
#include <boost/optional.hpp>
#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/Cancellable.hpp>
#include <OpenP2P/Stream.hpp>

namespace OpenP2P{

	class BinaryStream: public Cancellable{
		public:
			BinaryStream(Stream&);

			inline operator bool(){
				return isValid_;
			}

			inline void setValid(){
				isValid_ = true;
			}

			inline void setInvalid(){
				isValid_ = false;
			}

			bool read(uint8_t *, std::size_t);

			bool write(const uint8_t *, std::size_t);

			std::size_t tryRead(uint8_t *, std::size_t);

			std::size_t tryWrite(const uint8_t *, std::size_t);

			void cancel();

		private:
			Stream& stream_;
			bool isValid_;

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
	BinaryStream& operator<<(BinaryStream&, const Buffer&);


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
	BinaryStream& operator>>(BinaryStream&, Buffer&);

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
