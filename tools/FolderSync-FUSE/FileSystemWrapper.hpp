#ifndef OPENP2P_FOLDERSYNC_FILESYSTEMWRAPPER_HPP
#define OPENP2P_FOLDERSYNC_FILESYSTEMWRAPPER_HPP

#include <stddef.h>

#include <string>
#include <vector>

#include <FUSE/FileSystem.hpp>
#include <FUSE/Handle.hpp>
#include <FUSE/Path.hpp>

namespace OpenP2P {

	namespace FolderSync {
	
		class NodeSystem;
		
		class FileSystemWrapper: public FUSE::FileSystem {
			public:
				FileSystemWrapper(NodeSystem& nodeSystem);
				
				FUSE::Handle openFile(const FUSE::Path& path);
				
				void closeFile(FUSE::Handle handle);
				
				size_t readFile(FUSE::Handle handle, size_t offset, uint8_t* buffer, size_t size) const;
				
				size_t writeFile(FUSE::Handle handle, size_t offset, const uint8_t* buffer, size_t size);
				
				void resize(const FUSE::Path& path, size_t size);
				
				FUSE::Handle openDirectory(const FUSE::Path& path);
				
				void closeDirectory(FUSE::Handle handle);
				
				std::vector<std::string> readDirectory(FUSE::Handle handle) const;
				
				FUSE::Handle createAndOpenFile(const FUSE::Path& path, mode_t mode);
				
				void removeFile(const FUSE::Path& path);
				
				void createDirectory(const FUSE::Path& path, mode_t mode);
				
				void removeDirectory(const FUSE::Path& path);
				
				void rename(const FUSE::Path& sourcePath, const FUSE::Path& destPath);
				
				struct stat getAttributes(const FUSE::Path& path) const;
				
				void changeMode(const FUSE::Path& path, mode_t mode);
				
				void changeOwner(const FUSE::Path& path, uid_t user, gid_t group);
				
			private:
				// Non-copyable.
				FileSystemWrapper(const FileSystemWrapper&) = delete;
				FileSystemWrapper& operator=(FileSystemWrapper) = delete;
				
				NodeSystem& nodeSystem_;
				
		};
		
	}
	
}

#endif
