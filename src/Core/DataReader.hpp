#ifndef OPENP2P_CORE_DATAREADER_HPP
#define OPENP2P_CORE_DATAREADER_HPP

#include <string>
#include "Types.hpp"

namespace openp2p{

	namespace Core{

		class DataSource;

		class DataReader{
			public:
				DataReader(DataSource&);

				DataReader& operator>>(Uint8&);

				DataReader& operator>>(Uint16&);

				DataReader& operator>>(Uint32&);

				DataReader& operator>>(std::string&);

			private:
				DataSource& mSource;

		};

	}

}

#endif
