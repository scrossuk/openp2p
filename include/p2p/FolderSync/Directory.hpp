#ifndef P2P_FOLDERSYNC_DIRECTORY_HPP
#define P2P_FOLDERSYNC_DIRECTORY_HPP

#include <string>
#include <vector>

#include <p2p/FolderSync/BlockId.hpp>
#include <p2p/FolderSync/Node.hpp>

namespace p2p {
	
	namespace FolderSync {
		
		class Directory {
			public:
				Directory(Node& node);
				~Directory();
				
				size_t childCount() const;
				
				void addChild(const std::string& name, const BlockId& blockId);
				
				void forceAddChild(const std::string& name, const BlockId& blockId);
				
				void updateChild(const std::string& name, const BlockId& blockId);
				
				void removeChild(const std::string& name);
				
				bool hasChild(const std::string& name) const;
				
				BlockId getChild(const std::string& name) const;
				
				std::vector<std::string> childNames() const;
				
			private:
				Node& node_;
			
		};
		
	}
	
}

#endif
