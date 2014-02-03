#include <assert.h>
#include <climits>
#include <cstdarg>
#include <cstring>
#include <ctime>

#include <fstream>
#include <set>
#include <string>
#include <unordered_map>

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <OpenP2P/FolderSync/Block.hpp>
#include <OpenP2P/FolderSync/BlockId.hpp>
#include <OpenP2P/FolderSync/Database.hpp>
#include <OpenP2P/FolderSync/Directory.hpp>
#include <OpenP2P/FolderSync/FileDatabase.hpp>
#include <OpenP2P/FolderSync/Node.hpp>

#include <FUSE/ErrorException.hpp>
#include <FUSE/FileSystem.hpp>
#include <FUSE/Path.hpp>

#include "FileSystemWrapper.hpp"
#include "HandleRef.hpp"
#include "NodeSystem.hpp"

namespace OpenP2P {

	namespace FolderSync {
		
		FileSystemWrapper::FileSystemWrapper(NodeSystem& nodeSystem)
			: nodeSystem_(nodeSystem) { }
		
		FUSE::Handle FileSystemWrapper::openFile(const FUSE::Path& path) {
			auto handle = openPath(nodeSystem_, path);
			
			const auto type = nodeSystem_.nodeType(handle.get());
			if (type != TYPE_FILE) {
				throw FUSE::ErrorException(EISDIR);
			}
			
			return handle.release();
		}
		
		void FileSystemWrapper::closeFile(FUSE::Handle handle) {
			const auto type = nodeSystem_.nodeType(handle);
			if (type != TYPE_FILE) {
				throw FUSE::ErrorException(EISDIR);
			}
			
			auto handleRef = HandleRef(nodeSystem_, handle);
			
			// Just in case FUSE didn't call 'flush'.
			nodeSystem_.flushNode(handleRef.get());
		}
		
		size_t FileSystemWrapper::readFile(FUSE::Handle handle, size_t offset, uint8_t* buffer, size_t size) const {
			return nodeSystem_.readFile(handle, offset, buffer, size);
		}
		
		size_t FileSystemWrapper::writeFile(FUSE::Handle handle, size_t offset, const uint8_t* buffer, size_t size) {
			return nodeSystem_.writeFile(handle, offset, buffer, size);
		}
		
		void FileSystemWrapper::resize(const FUSE::Path& path, size_t size) {
			const auto handle = openPath(nodeSystem_, path);
			nodeSystem_.resizeFile(handle.get(), size);
		}
		
		FUSE::Handle FileSystemWrapper::openDirectory(const FUSE::Path& path) {
			auto handle = openPath(nodeSystem_, path);
			
			const auto type = nodeSystem_.nodeType(handle.get());
			if (type != TYPE_DIRECTORY) {
				throw FUSE::ErrorException(ENOTDIR);
			}
			
			return handle.release();
		}
		
		void FileSystemWrapper::closeDirectory(FUSE::Handle handle) {
			const auto type = nodeSystem_.nodeType(handle);
			if (type != TYPE_DIRECTORY) {
				throw FUSE::ErrorException(ENOTDIR);
			}
			
			auto handleRef = HandleRef(nodeSystem_, handle);
			
			// Just in case FUSE didn't call 'flush'.
			nodeSystem_.flushNode(handleRef.get());
		}
		
		std::vector<std::string> FileSystemWrapper::readDirectory(FUSE::Handle handle) const {
			return nodeSystem_.readDirectory(handle);
		}
		
		FUSE::Handle FileSystemWrapper::createAndOpenFile(const FUSE::Path& path, mode_t) {
			if (path.empty()) {
				throw FUSE::ErrorException(EINVAL);
			}
			
			// Create empty file in parent directory.
			auto parent = openPath(nodeSystem_, path.parent());
			if (nodeSystem_.nodeType(parent.get()) != TYPE_DIRECTORY) {
				throw FUSE::ErrorException(ENOTDIR);
			}
			
			nodeSystem_.addChild(parent.get(), path.back(), false);
			
			nodeSystem_.flushNode(parent.get());
			
			// Open empty file.
			return openFile(path);
		}
		
		void FileSystemWrapper::removeFile(const FUSE::Path& path) {
			if (path.empty()) {
				throw FUSE::ErrorException(EINVAL);
			}
			
			auto parent = openPath(nodeSystem_, path.parent());
			if (nodeSystem_.nodeType(parent.get()) != TYPE_DIRECTORY) {
				throw FUSE::ErrorException(ENOTDIR);
			}
			
			// Check child is a file.
			auto child = HandleRef(nodeSystem_, nodeSystem_.openChild(parent.get(), path.back()));
			if (nodeSystem_.nodeType(child.get()) != TYPE_FILE) {
				throw FUSE::ErrorException(EISDIR);
			}
			
			nodeSystem_.removeChild(parent.get(), path.back());
			
			nodeSystem_.flushNode(parent.get());
		}
		
		void FileSystemWrapper::createDirectory(const FUSE::Path& path, mode_t) {
			if (path.empty()) {
				throw FUSE::ErrorException(EINVAL);
			}
			
			auto parent = openPath(nodeSystem_, path.parent());
			if (nodeSystem_.nodeType(parent.get()) != TYPE_DIRECTORY) {
				throw FUSE::ErrorException(ENOTDIR);
			}
			
			nodeSystem_.addChild(parent.get(), path.back(), true);
			
			nodeSystem_.flushNode(parent.get());
		}
		
		void FileSystemWrapper::removeDirectory(const FUSE::Path& path) {
			if (path.empty()) {
				throw FUSE::ErrorException(EINVAL);
			}
			
			auto parent = openPath(nodeSystem_, path.parent());
			if (nodeSystem_.nodeType(parent.get()) != TYPE_DIRECTORY) {
				throw FUSE::ErrorException(ENOTDIR);
			}
			
			// Check child is an empty directory.
			auto child = HandleRef(nodeSystem_, nodeSystem_.openChild(parent.get(), path.back()));
			const auto childChildren = nodeSystem_.readDirectory(child.get());
			if (!childChildren.empty()) {
				throw FUSE::ErrorException(ENOTEMPTY);
			}
			
			nodeSystem_.removeChild(parent.get(), path.back());
			
			nodeSystem_.flushNode(parent.get());
		}
		
		void FileSystemWrapper::rename(const FUSE::Path& sourcePath, const FUSE::Path& destPath) {
			if (sourcePath.empty() || destPath.empty()) {
				// Can't rename to/from root.
				throw FUSE::ErrorException(EINVAL);
			}
			
			if (sourcePath.hasChild(destPath)) {
				// Can't rename to a subdirectory of itself.
				throw FUSE::ErrorException(EINVAL);
			}
			
			auto sourceHandle = openPath(nodeSystem_, sourcePath.parent());
			auto destHandle = openPath(nodeSystem_, destPath.parent());
			
			nodeSystem_.rename(sourceHandle.get(), sourcePath.back(), destHandle.get(), destPath.back());
			
			nodeSystem_.flushNode(sourceHandle.get());
			nodeSystem_.flushNode(destHandle.get());
		}
		
		struct stat FileSystemWrapper::getAttributes(const FUSE::Path& path) const {
			auto handle = openPath(nodeSystem_, path);
			return nodeSystem_.getAttributes(handle.get());
		}
		
		void FileSystemWrapper::changeMode(const FUSE::Path&, mode_t) {
			// Not implemented.
			throw FUSE::ErrorException(ENOSYS);
		}
		
		void FileSystemWrapper::changeOwner(const FUSE::Path&, uid_t, gid_t) {
			// Not implemented.
			throw FUSE::ErrorException(ENOSYS);
		}
		
	}
	
}

