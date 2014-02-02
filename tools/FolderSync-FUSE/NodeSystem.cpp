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

#include "NodeSystem.hpp"

namespace OpenP2P {

	namespace FolderSync {
	
		namespace {
		
			struct stat getNodeAttr(const Metadata& metadata, const Node& node) {
				struct stat s;
				memset(&s, 0, sizeof(s));
				
				// Mode = 755.
				s.st_mode = (S_IRUSR | S_IWUSR | S_IXUSR) | (S_IRGRP | S_IXGRP) | (S_IROTH | S_IXOTH);
				
				s.st_uid = geteuid();
				s.st_gid = getegid();
				s.st_atime = metadata.accessTime;
				s.st_mtime = metadata.modifyTime;
				
				switch (node.type()) {
					case FolderSync::TYPE_FILE: {
						s.st_size = node.size();
						s.st_nlink = 1;
						s.st_mode |= S_IFREG;
						s.st_blksize = BLOCK_SIZE;
						break;
					}
					
					case FolderSync::TYPE_DIRECTORY: {
						s.st_nlink = 2;
						s.st_mode |= S_IFDIR;
						break;
					}
					
					default:
						throw FUSE::ErrorException(EINVAL);
				}
				
				return s;
			}
			
		}
		
		NodeSystem::NodeSystem(Database& database)
			: database_(database), nextHandle_(0) {
			emptyDir_ = CreateEmptyNode(database_, TYPE_DIRECTORY);
			emptyFile_ = CreateEmptyNode(database_, TYPE_FILE);
			rootId_ = emptyDir_;
			metadata_.emplace(FUSE::Path(), Metadata::Now());
		}
		
		NodeInfo& NodeSystem::getNodeInfo(FUSE::Handle handle) {
			const auto iterator = handleToNodeMap_.find(path);
			
			if (iterator == handleToNodeMap_.end()) {
				throw FUSE::ErrorException(EBADFD);
			}
			
			return iterator->second;
		}
		
		const FUSE::Path& NodeSystem::getPath(FUSE::Handle handle) {
			const auto iterator = handleToPathMap_.find(handle);
			
			if (iterator == handleToPathMap_.end()) {
				// Node has presumably been detached.
				throw FUSE::ErrorException(ENOENT);
			}
			
			return iterator->second;
		}
		
		FUSE::Handle NodeSystem::openRoot() {
			const auto path = FUSE::Path();
			
			// Check if the node is already open.
			const auto iterator = pathToHandleMap_.find(path);
			
			if (iterator != pathToHandleMap_.end()) {
				const auto handle = iterator->second;
				getNodeInfo(handle).refCount++;
				return iterator->second;
			}
			
			// Create a node.
			auto nodeInfoPtr = unique_ptr<NodeInfo>(new NodeInfo(database_, rootId_, metadata_.at(path)));
			
			// Generate a new handle.
			const auto handle = nextHandle_++;
			
			handleToNodeMap_.emplace(handle, std::move(nodeInfoPtr));
			
			// Create the bi-directional mapping between handle and path.
			pathToHandleMap_.emplace(path, handle);
			handleToPathMap_.emplace(handle, path);
			
			return handle;
		}
		
		FUSE::Handle NodeSystem::openChild(FUSE::Handle parentHandle, const std::string& name) {
			auto& parentNodeInfo = getNodeInfo(parentHandle);
			
			if (parentNodeInfo.node.type() != TYPE_DIRECTORY) {
				throw FUSE::ErrorException(ENOTDIR);
			}
			
			// Get child path.
			const auto childPath = getPath(parentHandle) + name;
			
			// Check if the node is already open.
			const auto iterator = pathToHandleMap_.find(childPath);
			
			if (iterator != pathToHandleMap_.end()) {
				const auto handle = iterator->second;
				getNode(handle).refCount()++;
				return iterator->second;
			}
			
			// Get the child's block ID from the parent.
			Directory directory(parentNodeInfo.node);
			
			if (!directory.hasChild(name)) {
				throw FUSE::ErrorException(ENOENT);
			}
			
			const auto childBlockId = directory.getChild(name);
			
			// Create a node.
			auto nodeInfoPtr = unique_ptr<NodeInfo>(new NodeInfo(database_, childBlockId, metadata_.at(childPath)));
			
			// Generate a new handle.
			const auto handle = nextHandle_++;
			
			handleToNodeMap_.emplace(handle, std::move(nodeInfoPtr));
			
			// Create the bi-directional mapping between handle and path.
			pathToHandleMap_.emplace(path, handle);
			handleToPathMap_.emplace(handle, path);
			
			return handle;
		}
		
		void NodeSystem::closeNode(FUSE::Handle handle) {
			const auto nodeIterator = handleToNodeMap_.find(handle);
			
			if (nodeIterator == handleToNodeMap_.end()) {
				throw FUSE::ErrorException(EBADF);
			}
			
			auto& nodeInfo = nodeIterator->second;
			
			if (--(node.refCount) != 0) {
				return;
			}
			
			// Remove bi-directional mapping, if
			// any exists (node may have been detached).
			const auto pathIterator = handleToPathMap_.find(handle);
			
			if (pathIterator != handleToPathMap_.end()) {
				pathToHandleMap_.erase(pathIterator->second);
				handleToPathMap_.erase(pathIterator);
			}
			
			// Destroy node.
			handleToNodeMap_.erase(nodeIterator);
		}
		
		void NodeSystem::flushNode(FUSE::Handle handle) {
			auto& nodeInfo = getNode(handle);
			
			// Get the node's path; this won't be
			// available if it has been detached,
			// in which case nothing needs to be done.
			const auto pathIterator = handleToPathMap_.find(handle);
			
			if (pathIterator != handleToPathMap_.end()) {
				const auto& path = pathIterator->second;
				
				if (node.isModified()) {
					node.flush();
					
					nodeInfo.metadata.updateModifiedTime();
					
					// Perform a cascade update.
					journal_.commitNode(path, node.blockId());
				} else {
					nodeInfo.metadata.updateAccessTime();
				}
				
				metadata_.at(path) = nodeInfo.metadata;
			}
		}
		
		size_t NodeSystem::readFile(FUSE::Handle handle, size_t offset, uint8_t* buffer, size_t size) const {
			auto& nodeInfo = getNode(handle);
			
			if (node.type() != TYPE_FILE) {
				throw FUSE::ErrorException(EISDIR);
			}
			
			return node.read(offset, buffer, size);
		}
		
		size_t NodeSystem::writeFile(FUSE::Handle handle, size_t offset, const uint8_t* buffer, size_t size) {
			auto& nodeInfo = getNode(handle);
			
			if (node.type() != TYPE_FILE) {
				throw FUSE::ErrorException(EISDIR);
			}
			
			return node.write(offset, buffer, size);
		}
		
		void NodeSystem::resizeFile(FUSE::Handle handle, size_t size) {
			auto& nodeInfo = getNode(handle);
			
			if (node.type() != TYPE_FILE) {
				throw FUSE::ErrorException(EISDIR);
			}
			
			node.resize(size);
		}
		
		std::vector<std::string> NodeSystem::readDirectory(FUSE::Handle handle) const {
			auto& nodeInfo = getNodeInfo(handle);
			
			if (node.type() != TYPE_DIRECTORY) {
				throw FUSE::ErrorException(ENOTDIR);
			}
			
			Directory directory(nodeInfo.node);
			return directory.readNames();
		}
		
		void NodeSystem::addChild(FUSE::Handle handle, const std::string& name, bool isDirectory) {
			auto& nodeInfo = getNodeInfo(handle);
			
			if (node.type() != TYPE_DIRECTORY) {
				throw FUSE::ErrorException(ENOTDIR);
			}
			
			const auto childPath = getPath(handle) + name;
			
			Directory directory(nodeInfo.node);
			
			if (directory.hasChild(name)) {
				throw FUSE::ErrorException(EEXIST);
			}
			
			directory.addChild(name, isDirectory ? emptyDir_ : emptyFile_);
			
			// Add metadata information.
			metadata_.emplace(childPath, Metadata::Now());
			
			// Flush to trigger immediate cascade updates.
			flushNode(handle);
		}
		
		void NodeSystem::removeChild(FUSE::Handle handle, const std::string& name) {
			auto& parentNodeInfo = getNodeInfo(handle);
			
			if (parentNodeInfo.node.type() != TYPE_DIRECTORY) {
				throw FUSE::ErrorException(ENOTDIR);
			}
			
			const auto childPath = getPath(handle) + name;
			
			Directory directory(nodeInfo.node);
			
			if (!directory.hasChild(name)) {
				throw FUSE::ErrorException(ENOENT);
			}
			
			// Remove child.
			directory.removeChild(name);
			
			// Detach child if it's currently open.
			const auto handleIterator = pathToHandleMap_.find(childPath);
			
			if (handleIterator != pathToHandleMap_.end()) {
				handleToPathMap_.erase(handleIterator->second);
				pathToHandleMap_.erase(handleIterator);
			}
		}
		
		void NodeSystem::rename(const Path& sourcePath, const Path& destPath) {
			// TODO.
			throw FUSE::ErrorException(ENOSYS);
		}
		
		struct stat NodeSystem::getAttributes(FUSE::Handle handle) const {
			auto& parentNodeInfo = getNodeInfo(handle);
			return getNodeAttr(parentNodeInfo.metadata, parentNodeInfo.node);
		}
		
	}
	
}

