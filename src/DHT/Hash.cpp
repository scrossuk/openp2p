#include "../Core.hpp"
#include "Hash.hpp"

using namespace openp2p::Core;

namespace openp2p{

	namespace DHT{

		Hash Hash::Unserialize(BufferIterator& iterator){
			Hash hash;
			iterator.Read(hash.mData, SizeInBytes);
			return hash;
		}

		Hash::Hash(){
			//Generate random hash
		}

		Hash::Hash(Uint8 c){
			for(unsigned int i = 0; i < SizeInBytes; ++i){
				mData[i] = c;
			}
		}

		Hash::Hash(const Uint8 * data){
			for(unsigned int i = 0; i < SizeInBytes; ++i){
				mData[i] = data[i];
			}
		}

		Hash::Hash(const Hash& hash){
			const Uint8 * data = hash.Data();
			for(unsigned int i = 0; i < SizeInBytes; ++i){
				mData[i] = data[i];
			}
		}

		Hash Hash::operator^(const Hash& hash) const{
			const Uint8 * data1 = mData;
			const Uint8 * data2 = hash.mData;

			Uint8 resultData[SizeInBytes];

			for(unsigned int i = 0; i < SizeInBytes; ++i){
				resultData[i] = data1[i] ^ data2[i];
			}

			return Hash(resultData);
		}

		bool Hash::operator==(const Hash& hash) const{
			const Uint8 * data1 = mData;
			const Uint8 * data2 = hash.mData;

			for(unsigned int i = 0; i < SizeInBytes; ++i){
				if(data1[i] != data2[i]){
					return false;
				}
			}
			return true;
		}

		bool Hash::operator!=(const Hash& hash) const{
			return !(*this == hash);
		}

		bool Hash::operator<(const Hash& hash) const{
			const Uint8 * data1 = mData;
			const Uint8 * data2 = hash.mData;

			for(unsigned int i = 0; i < SizeInBytes; ++i){
				if(data1[i] != data2[i]){
					return data1[i] < data2[i];
				}
			}
			return false;
		}

		const Uint8 * Hash::Data() const{
			return mData;
		}

		Buffer Hash::Serialize() const{
			return Buffer(mData, SizeInBytes);
		}

	}

}


