#ifndef OPENP2P_FOLDERSYNC_BLOCKPATH_HPP
#define OPENP2P_FOLDERSYNC_BLOCKPATH_HPP

#include <stddef.h>

#include <vector>

#include <boost/functional/hash.hpp>

#include <OpenP2P/FolderSync/Constants.hpp>

namespace OpenP2P {
	
	namespace FolderSync {
		
		namespace {
			
			size_t indirectOffset(size_t blockIndex, size_t level) {
				size_t value = blockIndex - NODE_MAX_ROOT_REFS;
				for (size_t i = 0; i < level; i++) {
					value /= NODE_MAX_INDIRECT_REFS;
				}
				return value % NODE_MAX_INDIRECT_REFS;
			}
			
		}
		
		class BlockPath {
			public:
				static inline BlockPath Root() {
					return BlockPath();
				}
				
				static inline BlockPath Index(size_t blockIndex) {
					BlockPath path;
					auto& components = path.components_;
					
					if (blockIndex < BLOCK_COUNT(0)) {
						// Direct reference.
						components.push_back(blockIndex);
						return path;
					}
					
					if (blockIndex < BLOCK_COUNT(1)) {
						// Single indirect.
						components.push_back(NODE_SINGLE_INDIRECT_INDEX);
						components.push_back(indirectOffset(blockIndex, 0));
						return path;
					}
					
					if (blockIndex < BLOCK_COUNT(2)) {
						// Double indirect.
						components.push_back(NODE_DOUBLE_INDIRECT_INDEX);
						components.push_back(indirectOffset(blockIndex, 1));
						components.push_back(indirectOffset(blockIndex, 0));
						return path;
					}
					
					if (blockIndex < BLOCK_COUNT(3)) {
						// Triple indirect.
						components.push_back(NODE_TRIPLE_INDIRECT_INDEX);
						components.push_back(indirectOffset(blockIndex, 2));
						components.push_back(indirectOffset(blockIndex, 1));
						components.push_back(indirectOffset(blockIndex, 0));
						return path;
					}
					
					throw std::runtime_error("Block index exceeds triple indirect.");
				}
				
				inline bool isRoot() const {
					return components_.empty();
				}
				
				inline size_t size() const {
					return components_.size();
				}
				
				inline size_t at(size_t index) const {
					return components_.at(index);
				}
				
				inline size_t back() const {
					return components_.back();
				}
				
				inline BlockPath parent() const {
					BlockPath parentPath;
					parentPath.components_ = components_;
					parentPath.components_.pop_back();
					return parentPath;
				}
				
				inline bool hasChild(const BlockPath& path) const {
					if (size() <= path.size()) return false;
					
					for (size_t i = 0; i < path.size(); i++) {
						if (at(i) != path.at(i)) return false;
					}
					
					return true;
				}
				
				inline bool operator==(const BlockPath& path) const {
					if (size() != path.size()) return false;
					
					for (size_t i = 0; i < path.size(); i++) {
						if (at(i) != path.at(i)) return false;
					}
					
					return true;
				}
				
				inline std::size_t hash() const {
					return boost::hash_range(components_.begin(), components_.end());
				}
				
			private:
				inline BlockPath() { }
				
				std::vector<size_t> components_;
			
		};
		
	}
	
}

namespace std {
	
	template<>
	struct hash<OpenP2P::FolderSync::BlockPath> {
		std::size_t operator()(const OpenP2P::FolderSync::BlockPath& path) const {
			return path.hash();
		}
	};
	
}

#endif
