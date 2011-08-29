#ifndef OPENP2P_BINARYSTREAM_HPP
#define OPENP2P_BINARYSTREAM_HPP

#include <vector>
#include <boost/array.hpp>
#include <boost/optional.hpp>
#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/Stream.hpp>

namespace OpenP2P{

	class BinaryIStream{
		public:
			inline BinaryIStream(IStream& stream) : stream_(stream), isValid_(true){ }

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

			std::size_t tryRead(uint8_t *, std::size_t);
			
			inline IStream& getIStream(){
				return stream_;
			}

		private:
			IStream& stream_;
			bool isValid_;

	};

	class BinaryOStream: public Cancellable{
		public:
			inline BinaryOStream(OStream& stream) : stream_(stream), isValid_(true){ }

			inline operator bool(){
				return isValid_;
			}

			inline void setValid(){
				isValid_ = true;
			}

			inline void setInvalid(){
				isValid_ = false;
			}

			bool write(const uint8_t *, std::size_t);

			std::size_t tryWrite(const uint8_t *, std::size_t);

			inline OStream& getOStream(){
				return stream_;
			}

		private:
			OStream& stream_;
			bool isValid_;

	};

	class BinaryIOStream: public BinaryIStream, public BinaryOStream{
		public:
			inline BinaryIOStream(IOStream& stream) : BinaryIStream(stream), BinaryOStream(stream){ }

			inline operator bool(){
				return BinaryIStream::operator bool() && BinaryOStream::operator bool();
			}

	};

	BinaryIStream& operator>>(BinaryIStream&, bool&);
	BinaryIStream& operator>>(BinaryIStream&, uint8_t&);
	BinaryIStream& operator>>(BinaryIStream&, int8_t&);
	BinaryIStream& operator>>(BinaryIStream&, uint16_t&);
	BinaryIStream& operator>>(BinaryIStream&, int16_t&);
	BinaryIStream& operator>>(BinaryIStream&, uint32_t&);
	BinaryIStream& operator>>(BinaryIStream&, int32_t&);
	BinaryIStream& operator>>(BinaryIStream&, uint64_t&);
	BinaryIStream& operator>>(BinaryIStream&, int64_t&);
	BinaryIStream& operator>>(BinaryIStream&, std::string&);
	BinaryIStream& operator>>(BinaryIStream&, Buffer&);
	BinaryIStream& operator>>(BinaryIStream&, OStream&);

	template <typename T, std::size_t N>
	BinaryIStream& operator>>(BinaryIStream& stream, boost::array<T, N>& array){
		for(std::size_t i = 0; i < N; i++){
			stream >> array[i];
		}
		return stream;
	}

	template <typename T>
	BinaryIStream& operator>>(BinaryIStream& stream, std::vector<T>& array){
		for(std::size_t i = 0; i < array.size(); i++){
			stream >> array[i];
		}
		return stream;
	}

	template <typename T>
	BinaryIStream& operator>>(BinaryIStream& stream, boost::optional<T>& optional){
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

	BinaryOStream& operator<<(BinaryOStream&, bool);
	BinaryOStream& operator<<(BinaryOStream&, uint8_t);
	BinaryOStream& operator<<(BinaryOStream&, int8_t);
	BinaryOStream& operator<<(BinaryOStream&, uint16_t);
	BinaryOStream& operator<<(BinaryOStream&, int16_t);
	BinaryOStream& operator<<(BinaryOStream&, uint32_t);
	BinaryOStream& operator<<(BinaryOStream&, int32_t);
	BinaryOStream& operator<<(BinaryOStream&, uint64_t);
	BinaryOStream& operator<<(BinaryOStream&, int64_t);
	BinaryOStream& operator<<(BinaryOStream&, const char *);
	BinaryOStream& operator<<(BinaryOStream&, const std::string&);
	BinaryOStream& operator<<(BinaryOStream&, const Buffer&);


	template <typename T, std::size_t N>
	BinaryOStream& operator<<(BinaryOStream& stream, const boost::array<T, N>& array){
		for(std::size_t i = 0; i < N; i++){
			stream << array[i];
		}
		return stream;
	}

	template <typename T>
	BinaryOStream& operator<<(BinaryOStream& stream, const std::vector<T>& array){
		for(std::size_t i = 0; i < array.size(); i++){
			stream << array[i];
		}
		return stream;
	}

	template <class T>
	BinaryOStream& operator<<(BinaryOStream& stream, const boost::optional<T>& optional){
		if(optional){
			stream << true << *optional;
		}else{
			stream << false;
		}
		return stream;
	}

}

#endif
