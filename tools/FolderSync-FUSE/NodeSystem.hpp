#include <assert.h>
#include <climits>
#include <cstdarg>
#include <cstring>
#include <ctime>

#include <string>
#include <unordered_map>

#include <OpenP2P/FolderSync/BlockId.hpp>
#include <OpenP2P/FolderSync/Database.hpp>
#include <OpenP2P/FolderSync/Node.hpp>

#include <FUSE/FileSystem.hpp>
#include <FUSE/Path.hpp>

namespace OpenP2P {

	namespace FolderSync {
	
		struct Metadata {
			time_t accessTime, modifyTime;
			
			inline Metadata()
				: accessTime(0), modifyTime(0) { }
			
			static inline Metadata Now() {
				Metadata metadata;
				metadata.accessTime = time(0);
				metadata.modifyTime = time(0);
				return metadata;
			}
			
			inline void updateAccessTime() {
				accessTime = time(0);
			}
			
			inline void updateModifyTime() {
				modifyTime = time(0);
			}
			
		};
		
		struct NodeInfo {
			size_t refCount;
			Node node;
			Metadata metadata;
			
			inline NodeInfo(Database& database, const BlockId& blockId, const Metadata& pMetadata)
				: refCount(1), node(database, blockId), metadata(pMetadata) { }
		};
		
		class NodeSystem {
			public:
				NodeSystem(Database& database);
				
				FUSE::Handle openRoot();
				
				FUSE::Handle openChild(FUSE::Handle handle, const std::string& name);
				
				void closeNode(FUSE::Handle handle);
				
				void flushNode(FUSE::Handle handle);
				
				NodeType nodeType(FUSE::Handle handle);
				
				size_t readFile(FUSE::Handle handle, size_t offset, uint8_t* buffer, size_t size) const;
				
				size_t writeFile(FUSE::Handle handle, size_t offset, const uint8_t* buffer, size_t size);
				
				void resizeFile(FUSE::Handle handle, size_t size);
				
				std::vector<std::string> readDirectory(FUSE::Handle handle) const;
				
				void addChild(FUSE::Handle handle, const std::string& name, bool isDirectory);
				
				void removeChild(FUSE::Handle handle, const std::string& name);
				
				void rename(const FUSE::Path& sourcePath, const FUSE::Path& destPath);
				
				struct stat getAttributes(FUSE::Handle handle) const;
				
			private:
				// Non-copyable.
				NodeSystem(const NodeSystem&) = delete;
				NodeSystem& operator=(NodeSystem) = delete;
				
				Database& database_;
				std::unordered_map<FUSE::Path, Metadata> metadata_;
				BlockId emptyDir_, emptyFile_;
				BlockId rootId_;
				FUSE::Handle nextHandle_;
				std::unordered_map<FUSE::Handle, std::unique_ptr<NodeInfo>> handleToNodeMap_;
				std::unordered_map<FUSE::Handle, FUSE::Path> handleToPathMap_;
				std::unordered_map<FUSE::Path, FUSE::Handle> pathToHandleMap_;
				
		};
		
	}
	
}

