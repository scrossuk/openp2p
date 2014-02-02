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
#include "NodeSystem.hpp"

namespace OpenP2P {

	namespace FolderSync {
	
		namespace {
			
			class HandleRef {
				public:
					HandleRef()
						: nodeSystem_(nullptr), handle_(0) { }
					
					HandleRef(NodeSystem& nodeSystem, FUSE::Handle handle)
						: nodeSystem_(&nodeSystem), handle_(handle) {
							assert(nodeSystem_ != nullptr);
						}
					
					HandleRef(HandleRef&& ref)
						: HandleRef() {
						std::swap(nodeSystem_, ref.nodeSystem_);
						std::swap(handle_, ref.handle_);
					}
					
					~HandleRef() {
						if (nodeSystem_ == nullptr) return;
						nodeSystem_->closeNode(handle_);
					}
					
					HandleRef& operator=(HandleRef ref) {
						std::swap(nodeSystem_, ref.nodeSystem_);
						std::swap(handle_, ref.handle_);
						return *this;
					}
					
					FUSE::Handle get() const {
						assert(nodeSystem_ != nullptr);
						return handle_;
					}
					
					FUSE::Handle release() {
						assert(nodeSystem_ != nullptr);
						nodeSystem_ = nullptr;
						return handle_;
					}
					
				private:
					// Non-copyable.
					HandleRef(const HandleRef&) = delete;
					
					NodeSystem* nodeSystem_;
					FUSE::Handle handle_;
					
			};
			
			HandleRef openPath(NodeSystem& nodeSystem, const FUSE::Path& path) {
				auto handle = HandleRef(nodeSystem, nodeSystem.openRoot());
				for (size_t i = 0; i < path.size(); i++) {
					handle = HandleRef(nodeSystem, nodeSystem.openChild(handle.get(), path.at(i)));
				}
				return handle;
			}
			
		}
		
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
			nodeSystem_.readFile(handle, offset, buffer, size);
		}
		
		size_t FileSystemWrapper::writeFile(FUSE::Handle handle, size_t offset, const uint8_t* buffer, size_t size) {
			nodeSystem_.writeFile(handle, offset, buffer, size);
		}
		
		void FileSystemWrapper::resizeFile(FUSE::Handle handle, size_t size) {
			nodeSystem_.resizeFile(handle, size);
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
		
		FUSE::Handle FileSystemWrapper::createAndOpenFile(const FUSE::Path& path, mode_t mode) {
			if (path.empty()) {
				throw FUSE::ErrorException(EINVAL);
			}
			
			// Create empty file in parent directory.
			auto parent = openPath(nodeSystem_, path.parent());
			if (nodeSystem_.nodeType(parent.get()) != TYPE_DIRECTORY) {
				throw FUSE::ErrorException(ENOTDIR);
			}
			
			nodeSystem_.addChild(parent.get(), path.last(), false);
			
			// Open empty file.
			return openFile(path, mode);
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
			auto child = HandleRef(nodeSystem_, nodeSystem_.openChild(parent, path.last()));
			if (nodeSystem_.nodeType(child.get()) != TYPE_FILE) {
				throw FUSE::ErrorException(EISDIR);
			}
			
			nodeSystem_.removeChild(parent.get(), path.last());
		}
		
		void FileSystemWrapper::createDirectory(const FUSE::Path& path, mode_t mode) {
			if (path.empty()) {
				throw FUSE::ErrorException(EINVAL);
			}
			
			auto parent = openPath(nodeSystem_, path.parent());
			if (nodeSystem_.nodeType(parent.get()) != TYPE_DIRECTORY) {
				throw FUSE::ErrorException(ENOTDIR);
			}
			
			nodeSystem_.addChild(parent.get(), path.last(), true);
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
			auto child = HandleRef(nodeSystem_, nodeSystem_.openChild(parent, path.last()));
			const auto childChildren = nodeSystem_.readDirectory(child.get());
			if (!childChildren.empty()) {
				throw FUSE::ErrorException(ENOTEMPTY);
			}
			
			nodeSystem_.removeChild(parent.get(), path.last());
		}
		
		void FileSystemWrapper::rename(const FUSE::Path& sourcePath, const FUSE::Path& destPath) {
			
		}
		
		struct stat FileSystemWrapper::getAttributes(const FUSE::Path& path) const {
			auto handle = openPath(nodeSystem_, path);
			return nodeSystem_.getAttributes(handle.get());
		}
		
		void FileSystemWrapper::changeMode(const FUSE::Path& path, mode_t mode) {
			// Not implemented.
			throw FUSE::ErrorException(ENOSYS);
		}
		
		void FileSystemWrapper::changeOwner(const FUSE::Path& path, uid_t user, gid_t group) {
			// Not implemented.
			throw FUSE::ErrorException(ENOSYS);
		}
		
	}
	
}

