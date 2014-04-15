#ifndef OPENP2P_KADEMLIA_ID_HPP
#define OPENP2P_KADEMLIA_ID_HPP

#include <assert.h>
#include <stdint.h>

#include <array>
#include <cstddef>

namespace OpenP2P {

	namespace Kademlia {
	
		template <size_t ID_SIZE>
		struct Id {
			constexpr size_t ID_SIZE_IN_BITS = ID_SIZE * 8;
			
			std::array<uint8_t, ID_SIZE> data;
			
			static Id<ID_SIZE> Zero() {
				Id<ID_SIZE> id;
				id.data.fill(0x00);
				return id;
			}
			
			static Id<ID_SIZE> Max() {
				return ~(Zero());
			}
			
			bool bitAt(size_t index) const {
				const auto bitsInByte = 8;
				assert(index < (ID_SIZE * bitsInByte));
				const auto byte = data.at(index / bitsInByte);
				const auto bitIndex = bitsInByte - 1 - (index % bitsInByte);
				return ((byte >> bitIndex) & 1) == 1;
			}
		};
		
		template <size_t ID_SIZE>
		bool operator==(const Id<ID_SIZE>& a, const Id<ID_SIZE>& b) {
			for (size_t i = 0; i < ID_SIZE; i++) {
				if (a.data.at(i) != b.data.at(i)) {
					return false;
				}
			}
			
			return true;
		}
		
		template <size_t ID_SIZE>
		bool operator!=(const Id<ID_SIZE>& a, const Id<ID_SIZE>& b) {
			return !(a == b);
		}
		
		template <size_t ID_SIZE>
		Id<ID_SIZE> operator^(const Id<ID_SIZE>& a, const Id<ID_SIZE>& b) {
			Id<ID_SIZE> result;
			
			for (size_t i = 0; i < ID_SIZE; i++) {
				result.data[i] = a.data[i] ^ b.data[i];
			}
			
			return result;
		}
		
		template <size_t ID_SIZE>
		Id<ID_SIZE> operator~(const Id<ID_SIZE>& other) {
			Id<ID_SIZE> reverse;
			
			for (size_t i = 0; i < ID_SIZE; ++i) {
				reverse.data.at(i) = ~(other.data.at(i));
			}
			
			return reverse;
		}
		
		template <size_t ID_SIZE>
		bool operator<(const Id<ID_SIZE>& a, const Id<ID_SIZE>& b) {
			for (size_t i = 0; i < ID_SIZE; i++) {
				if (a.data.at(i) != b.data.at(i)) {
					return a.data.at(i) < b.data.at(i);
				}
			}
			
			return false;
		}
		
	}
	
}

#endif
