#include <string>
#include "Types.hpp"
#include "DataSource.hpp"
#include "DataReader.hpp"

namespace openp2p{

	namespace Core{

		DataReader::DataReader(DataSource& source) : mSource(source){ }

		DataReader& DataReader::operator>>(Uint8& v){
			Uint8 s[1];
			mSource.Read(s, 1);
			v = s[0];
			return *this;
		}

		DataReader& DataReader::operator>>(Uint16& v){
			Uint8 s[2];
			mSource.Read(s, 2);
			v = (s[0] << 8) | s[1];
			return *this;
		}

		DataReader& DataReader::operator>>(Uint32& v){
			Uint8 s[4];
			mSource.Read(s, 4);
			v = (s[0] << 24) | (s[1] << 16) | (s[2] << 8) | s[3];
			return *this;
		}

		DataReader& DataReader::operator>>(std::string& str){
			Uint16 size;
			*this >> size;

			Uint8 data[size];

			mSource.Read(data, size);

			str.assign((const char *) data, (std::size_t) size);

			return *this;
		}

	}

}

