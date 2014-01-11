#ifndef OPENP2P_KADEMLIA_ID_HPP
#define OPENP2P_KADEMLIA_ID_HPP

#include <stdint.h>
#include <cstddef>
#include <boost/array.hpp>
#include <OpenP2P/BinaryStream.hpp>

namespace OpenP2P {

	namespace Kademlia {
	
		//Size given in bytes
		template <size_t IdSize>
		struct Id {
			boost::array<uint8_t, IdSize> data;
			
			static Id<IdSize> Zero() {
				Id<IdSize> id;
				
				for (size_t i = 0; i < IdSize; ++i) {
					id.data[i] = 0;
				}
				
				return id;
			}
			
			static Id<IdSize> Max() {
				return ~(Zero());
			}
		};
		
		template <size_t IdSize>
		bool operator==(const Id<IdSize>& a, const Id<IdSize>& b) {
			for (size_t i = 0; i < IdSize; i++) {
				if (a.data[i] != b.data[i]) {
					return false;
				}
			}
			
			return true;
		}
		
		template <size_t IdSize>
		bool operator!=(const Id<IdSize>& a, const Id<IdSize>& b) {
			return !(a == b);
		}
		
		template <size_t IdSize>
		Id<IdSize> operator^(const Id<IdSize>& a, const Id<IdSize>& b) {
			Id<IdSize> result;
			
			for (size_t i = 0; i < IdSize; i++) {
				result.data[i] = a.data[i] ^ b.data[i];
			}
			
			return result;
		}
		
		template <size_t IdSize>
		Id<IdSize> operator~(const Id<IdSize>& other) {
			Id<IdSize> reverse;
			
			for (size_t i = 0; i < IdSize; ++i) {
				reverse.data[i] = ~(other.data[i]);
			}
			
			return reverse;
		}
		
		template <size_t IdSize>
		bool operator<(const Id<IdSize>& a, const Id<IdSize>& b) {
			for (size_t i = 0; i < IdSize; i++) {
				if (a.data[i] != b.data[i]) {
					return a.data[i] < b.data[i];
				}
			}
			
			return false;
		}
		
		template <size_t IdSize>
		BinaryIStream& operator>>(BinaryIStream& stream, Id<IdSize>& id) {
			return stream >> id.data;
		}
		
		template <size_t IdSize>
		BinaryOStream& operator<<(BinaryOStream& stream, const Id<IdSize>& id) {
			return stream << id.data;
		}
		
	}
	
}

#endif
