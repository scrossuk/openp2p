#ifndef OPENP2P_KADEMLIA_ID_HPP
#define OPENP2P_KADEMLIA_ID_HPP

#include <stdint.h>
#include <cstddef>
#include <boost/array.hpp>
#include <OpenP2P/IStream.hpp>
#include <OpenP2P/OStream.hpp>

namespace OpenP2P{

	namespace Kademlia{

		template <std::size_t SizeInBytes>
		struct Id{
			boost::array<uint8_t, SizeInBytes> data;
		};

		template <std::size_t IdSize>
		Id<IdSize> ZeroId(){
			Id<IdSize> id;
			for(std::size_t i = 0; i < IdSize; ++i){
				id.data[i] = 0;
			}
			return id;
		}

		template <std::size_t IdSize>
		Id<IdSize> MaxId(){
			return ~(ZeroId<IdSize>());
		}

		template <std::size_t IdSize>
		bool operator==(const Id<IdSize>& a, const Id<IdSize>& b){
			for(std::size_t i = 0; i < IdSize; i++){
				if(a.data[i] != b.data[i]) return false;
			}
			return true;
		}

		template <std::size_t IdSize>
		bool operator!=(const Id<IdSize>& a, const Id<IdSize>& b){
			return !(a == b);
		}

		template <std::size_t IdSize>
		Id<IdSize> operator^(const Id<IdSize>& a, const Id<IdSize>& b){
			Id<IdSize> result;
			for(std::size_t i = 0; i < IdSize; i++){
				result.data[i] = a.data[i] ^ b.data[i];
			}
			return result;
		}

		template <std::size_t IdSize>
		Id<IdSize> operator~(const Id<IdSize>& other){
			Id<IdSize> reverse;
			for(std::size_t i = 0; i < IdSize; ++i){
				reverse.data[i] = ~(other.data[i]);
			}
			return reverse;
		}

		template <std::size_t IdSize>
		bool operator<(const Id<IdSize>& a, const Id<IdSize>& b){
			for(std::size_t i = 0; i < IdSize; i++){
				if(a.data[i] != b.data[i]) return a.data[i] < b.data[i];
			}
			return false;
		}

		template <std::size_t IdSize>
		IStream& operator>>(IStream& stream, Id<IdSize>& id){
			return stream >> id.data;
		}

		template <std::size_t IdSize>
		OStream& operator<<(OStream& stream, const Id<IdSize>& id){
			return stream << id.data;
		}

	}

}

#endif
