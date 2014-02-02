#include <assert.h>
#include <stddef.h>

#include <algorithm>
#include <array>
#include <functional>
#include <stdexcept>

#include <boost/functional/hash.hpp>

#include <OpenP2P/FolderSync/BlockPath.hpp>
#include <OpenP2P/FolderSync/Constants.hpp>

namespace OpenP2P {

	namespace FolderSync {
	
		namespace {
		
			template <size_t LEVEL>
			uint16_t indirectOffset(size_t blockIndex) {
				const size_t value = blockIndex - NODE_ROOT_MAX_DIRECT_REFS;
				return static_cast<uint16_t>((value / INT_POW(NODE_MAX_INDIRECT_REFS, LEVEL)) % NODE_MAX_INDIRECT_REFS);
			}
			
		}
		
		BlockPath BlockPath::Root() {
			return BlockPath();
		}
		
		BlockPath BlockPath::Index(size_t blockIndex) {
			std::array<uint16_t, NODE_MAX_BLOCK_PATH> components;
			components.fill(0);
			
			if (blockIndex < BLOCK_COUNT(0)) {
				// Direct reference.
				components.at(0) = blockIndex;
				return BlockPath(1, components);
			}
			
			if (blockIndex < BLOCK_COUNT(1)) {
				// Single indirect.
				components.at(0) = NODE_SINGLE_INDIRECT_INDEX;
				components.at(1) = indirectOffset<0>(blockIndex);
				return BlockPath(2, components);
			}
			
			if (blockIndex < BLOCK_COUNT(2)) {
				// Double indirect.
				components.at(0) = NODE_DOUBLE_INDIRECT_INDEX;
				components.at(1) = indirectOffset<1>(blockIndex);
				components.at(2) = indirectOffset<0>(blockIndex);
				return BlockPath(3, components);
			}
			
			if (blockIndex < BLOCK_COUNT(3)) {
				// Triple indirect.
				components.at(0) = NODE_TRIPLE_INDIRECT_INDEX;
				components.at(1) = indirectOffset<2>(blockIndex);
				components.at(2) = indirectOffset<1>(blockIndex);
				components.at(3) = indirectOffset<0>(blockIndex);
				return BlockPath(4, components);
			}
			
			throw std::runtime_error("Block index exceeds triple indirect.");
		}
		
		bool BlockPath::isRoot() const {
			return size() == 0;
		}
		
		size_t BlockPath::size() const {
			return static_cast<size_t>(size_);
		}
		
		size_t BlockPath::at(size_t index) const {
			assert(index < size());
			return components_.at(index);
		}
		
		size_t BlockPath::back() const {
			assert(size() > 0);
			return components_.at(size() - 1);
		}
		
		BlockPath BlockPath::parent() const {
			assert(size() > 0);
			return BlockPath(size() - 1, components_);
		}
		
		bool BlockPath::operator==(const BlockPath& path) const {
			if (size() != path.size()) {
				return false;
			}
			
			return components_ == path.components_;
		}
		
		std::size_t BlockPath::hash() const {
			return boost::hash_range(components_.begin(), components_.end());
		}
		
		BlockPath::BlockPath()
			: size_(0) {
			components_.fill(0);
		}
		
		BlockPath::BlockPath(uint8_t pSize, const std::array<uint16_t, NODE_MAX_BLOCK_PATH>& components)
			: size_(pSize), components_(components) {
			components_.fill(0);
			std::copy_n(components.begin(), size(), components_.begin());
			
			assert(components_.at(0) < NODE_ROOT_MAX_REFS);
			
			for (size_t i = 1; i < components_.size(); i++) {
				(void) i;
				assert(components_.at(i) < NODE_MAX_INDIRECT_REFS);
			}
		}
		
	}
	
}

