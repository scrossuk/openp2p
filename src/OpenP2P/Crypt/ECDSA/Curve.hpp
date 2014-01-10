#ifndef OPENP2P_CRYPT_ECDSA_CURVE_HPP
#define OPENP2P_CRYPT_ECDSA_CURVE_HPP

#include <cryptopp/oids.h>

namespace OpenP2P {

	namespace Crypt {
	
		namespace ECDSA {
		
			enum Curve {
				brainpoolP256r1,
				brainpoolP512r1
			};
			
		}
		
	}
	
}

#endif
