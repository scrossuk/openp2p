#include <assert.h>
#include <climits>
#include <cstdarg>
#include <cstring>
#include <ctime>

#include <fstream>
#include <set>
#include <stack>
#include <string>
#include <unordered_map>

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <OpenP2P/FolderSync/Block.hpp>
#include <OpenP2P/FolderSync/BlockId.hpp>
#include <OpenP2P/FolderSync/Database.hpp>
#include <OpenP2P/FolderSync/Directory.hpp>
#include <OpenP2P/FolderSync/Node.hpp>

#include <FUSE/ErrorException.hpp>
#include <FUSE/Handle.hpp>
#include <FUSE/Path.hpp>

#include "HandleRef.hpp"
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
		
		BlockId NodeSystem::cascadeUpdate(const FUSE::Path& path, BlockId newId) {
			printf("Performing cascade for path %s.\n", path.toString().c_str());
			
			std::stack<HandleRef> refStack;
			refStack.push(HandleRef(*this, openRoot()));
			
			for (size_t i = 1; i < path.size(); i++) {
				refStack.push(HandleRef(*this, openChild(refStack.top().get(), path.at(i - 1))));
			}
			
			BlockId blockId = newId;
			
			for (size_t i = 0; i < path.size(); i++) {
				const size_t pos = path.size() - i - 1;
				printf("Updating block ID for component %llu.\n",
					(unsigned long long) pos);
				printf("Updating block ID for component %llu: '%s'.\n",
					(unsigned long long) pos,
					path.at(pos).c_str());
				
				auto& parentNode = getNodeInfo(refStack.top().get()).node;
				Directory directory(parentNode);
				directory.updateChild(path.at(pos), blockId);
				
				parentNode.flush();
				
				blockId = parentNode.blockId();
				
				refStack.pop();
			}
			
			return blockId;
		}
		
		NodeInfo& NodeSystem::getNodeInfo(FUSE::Handle handle) const {
			const auto iterator = handleToNodeMap_.find(handle);
			
			if (iterator == handleToNodeMap_.end()) {
				throw FUSE::ErrorException(EBADFD);
			}
			
			return *(iterator->second);
		}
		
		const FUSE::Path& NodeSystem::getPath(FUSE::Handle handle) const {
			const auto iterator = handleToPathMap_.find(handle);
			
			if (iterator == handleToPathMap_.end()) {
				// Node has presumably been detached.
				throw FUSE::ErrorException(ENOENT);
			}
			
			return iterator->second;
		}
		
		void NodeSystem::moveAllMetadata(const FUSE::Path& oldPath, const FUSE::Path& newPath) {
			assert(oldPath != newPath && !oldPath.hasChild(newPath));
			for (auto iterator = metadata_.begin(); iterator != metadata_.end();) {
				const auto& metadataPath = iterator->first;
				const auto metadata = iterator->second;
				const auto currentIterator = iterator;
				++iterator;
				
				if (metadataPath == oldPath || oldPath.hasChild(metadataPath)) {
					const auto newMetadataPath = metadataPath.rebase(oldPath, newPath);
					
					printf("Moving metadata at path %s to %s.\n",
						metadataPath.toString().c_str(),
						newMetadataPath.toString().c_str());
					
					metadata_.erase(currentIterator);
					metadata_.emplace(newMetadataPath, metadata);
				}
			}
		}
		
		void NodeSystem::removeAllMetadata(const FUSE::Path& removePath) {
			for (auto iterator = metadata_.begin(); iterator != metadata_.end();) {
				const auto& metadataPath = iterator->first;
				const auto currentIterator = iterator;
				++iterator;
				
				if (metadataPath == removePath || removePath.hasChild(metadataPath)) {
					printf("Removing metadata at path %s.\n",
						metadataPath.toString().c_str());
					
					metadata_.erase(currentIterator);
				}
			}
		}
		
		void NodeSystem::moveAllHandles(const FUSE::Path& oldPath, const FUSE::Path& newPath) {
			assert(oldPath != newPath && !oldPath.hasChild(newPath));
			for (auto& pair: handleToPathMap_) {
				const auto handle = pair.first;
				auto& nodePath = pair.second;
				
				if (nodePath == oldPath || oldPath.hasChild(nodePath)) {
					const auto newNodePath = nodePath.rebase(oldPath, newPath);
					
					printf("Moving handle %llu at path %s to %s.\n",
						(unsigned long long) handle,
						nodePath.toString().c_str(),
						newNodePath.toString().c_str());
					
					// Remove the current 'old path -> handle' mapping
					// and add a 'new path -> handle' mapping.
					pathToHandleMap_.erase(nodePath);
					pathToHandleMap_.emplace(newNodePath, handle);
					
					// Update path for the handle.
					nodePath = newNodePath;
				}
			}
		}
		
		void NodeSystem::detachAllHandles(const FUSE::Path& detachPath) {
			for (auto iterator = pathToHandleMap_.begin(); iterator != pathToHandleMap_.end();) {
				const auto& nodePath = iterator->first;
				const auto handle = iterator->second;
				const auto currentIterator = iterator;
				++iterator;
				
				if (nodePath == detachPath || detachPath.hasChild(nodePath)) {
					printf("Detaching handle %llu at path %s.\n",
						(unsigned long long) handle,
						nodePath.toString().c_str());
					
					// Remove bi-directional mapping.
					handleToPathMap_.erase(handle);
					pathToHandleMap_.erase(currentIterator);
				}
			}
		}
		
		FUSE::Handle NodeSystem::openRoot() {
			printf("Opening root.\n");
			const auto path = FUSE::Path();
			
			// Check if the node is already open.
			const auto iterator = pathToHandleMap_.find(path);
			
			if (iterator != pathToHandleMap_.end()) {
				const auto handle = iterator->second;
				getNodeInfo(handle).refCount++;
				return iterator->second;
			}
			
			// Create a node.
			auto nodeInfoPtr = std::unique_ptr<NodeInfo>(new NodeInfo(database_, rootId_, metadata_.at(path)));
			
			// Generate a new handle.
			const auto handle = nextHandle_++;
			
			handleToNodeMap_.emplace(handle, std::move(nodeInfoPtr));
			
			// Create the bi-directional mapping between handle and path.
			pathToHandleMap_.emplace(path, handle);
			handleToPathMap_.emplace(handle, path);
			
			return handle;
		}
		
		FUSE::Handle NodeSystem::openChild(FUSE::Handle parentHandle, const std::string& name) {
			printf("Opening child '%s' of directory with handle %llu.\n",
				name.c_str(), (unsigned long long) parentHandle);
			
			auto& parentNodeInfo = getNodeInfo(parentHandle);
			
			if (parentNodeInfo.node.type() != TYPE_DIRECTORY) {
				throw FUSE::ErrorException(ENOTDIR);
			}
			
			// Get child path.
			const auto path = getPath(parentHandle) + name;
			
			// Check if the node is already open.
			const auto iterator = pathToHandleMap_.find(path);
			
			if (iterator != pathToHandleMap_.end()) {
				const auto handle = iterator->second;
				getNodeInfo(handle).refCount++;
				return iterator->second;
			}
			
			// Get the child's block ID from the parent.
			Directory directory(parentNodeInfo.node);
			
			if (!directory.hasChild(name)) {
				throw FUSE::ErrorException(ENOENT);
			}
			
			const auto childBlockId = directory.getChild(name);
			
			// Create a node.
			auto nodeInfoPtr = std::unique_ptr<NodeInfo>(new NodeInfo(database_, childBlockId, metadata_.at(path)));
			
			// Generate a new handle.
			const auto handle = nextHandle_++;
			
			printf("Allocating handle %llu.\n",
				(unsigned long long) handle);
			
			handleToNodeMap_.emplace(handle, std::move(nodeInfoPtr));
			
			// Create the bi-directional mapping between handle and path.
			pathToHandleMap_.emplace(path, handle);
			handleToPathMap_.emplace(handle, path);
			
			return handle;
		}
		
		void NodeSystem::closeNode(FUSE::Handle handle) {
			printf("Closing handle %llu.\n",
				(unsigned long long) handle);
			
			const auto nodeIterator = handleToNodeMap_.find(handle);
			
			if (nodeIterator == handleToNodeMap_.end()) {
				throw FUSE::ErrorException(EBADF);
			}
			
			auto& nodeInfo = *(nodeIterator->second);
			
			if (--(nodeInfo.refCount) != 0) {
				return;
			}
			
			printf("Releasing handle %llu.\n",
				(unsigned long long) handle);
			
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
			printf("Flushing handle %llu.\n",
				(unsigned long long) handle);
			
			auto& nodeInfo = getNodeInfo(handle);
			
			// Get the node's path; this won't be
			// available if it has been detached,
			// in which case nothing needs to be done.
			const auto pathIterator = handleToPathMap_.find(handle);
			
			if (pathIterator != handleToPathMap_.end()) {
				const auto& path = pathIterator->second;
				
				if (nodeInfo.node.hasChanged()) {
					nodeInfo.metadata.updateModifyTime();
					
					// Perform a cascade update.
					nodeInfo.node.flush();
					rootId_ = cascadeUpdate(path, nodeInfo.node.blockId());
				} else {
					nodeInfo.metadata.updateAccessTime();
				}
				
				metadata_.at(path) = nodeInfo.metadata;
			}
		}
		
		NodeType NodeSystem::nodeType(FUSE::Handle handle) {
			return getNodeInfo(handle).node.type();
		}
		
		size_t NodeSystem::readFile(FUSE::Handle handle, size_t offset, uint8_t* buffer, size_t size) const {
			// printf("Read %llu bytes from handle %llu.\n", (unsigned long long) size, (unsigned long long) handle);
			
			auto& node = getNodeInfo(handle).node;
			
			if (node.type() != TYPE_FILE) {
				throw FUSE::ErrorException(EISDIR);
			}
			
			return node.read(offset, buffer, size);
		}
		
		size_t NodeSystem::writeFile(FUSE::Handle handle, size_t offset, const uint8_t* buffer, size_t size) {
			// printf("Write %llu bytes to handle %llu.\n", (unsigned long long) size, (unsigned long long) handle);
			
			auto& node = getNodeInfo(handle).node;
			
			if (node.type() != TYPE_FILE) {
				throw FUSE::ErrorException(EISDIR);
			}
			
			return node.write(offset, buffer, size);
		}
		
		void NodeSystem::resizeFile(FUSE::Handle handle, size_t size) {
			auto& node = getNodeInfo(handle).node;
			
			if (node.type() != TYPE_FILE) {
				throw FUSE::ErrorException(EISDIR);
			}
			
			node.resize(size);
		}
		
		std::vector<std::string> NodeSystem::readDirectory(FUSE::Handle handle) const {
			auto& node = getNodeInfo(handle).node;
			
			if (node.type() != TYPE_DIRECTORY) {
				throw FUSE::ErrorException(ENOTDIR);
			}
			
			Directory directory(node);
			return directory.childNames();
		}
		
		void NodeSystem::addChild(FUSE::Handle handle, const std::string& name, bool isDirectory) {
			auto& parentNode = getNodeInfo(handle).node;
			
			if (parentNode.type() != TYPE_DIRECTORY) {
				throw FUSE::ErrorException(ENOTDIR);
			}
			
			const auto path = getPath(handle) + name;
			
			Directory directory(parentNode);
			
			if (directory.hasChild(name)) {
				throw FUSE::ErrorException(EEXIST);
			}
			
			directory.addChild(name, isDirectory ? emptyDir_ : emptyFile_);
			
			// Add metadata information.
			metadata_.emplace(path, Metadata::Now());
		}
		
		void NodeSystem::removeChild(FUSE::Handle handle, const std::string& name) {
			auto& parentNode = getNodeInfo(handle).node;
			
			if (parentNode.type() != TYPE_DIRECTORY) {
				throw FUSE::ErrorException(ENOTDIR);
			}
			
			const auto path = getPath(handle) + name;
			
			Directory directory(parentNode);
			
			if (!directory.hasChild(name)) {
				throw FUSE::ErrorException(ENOENT);
			}
			
			// Remove child.
			directory.removeChild(name);
			
			// Remove metadata.
			removeAllMetadata(path);
			
			// Detach open handles.
			detachAllHandles(path);
		}
		
		void NodeSystem::rename(FUSE::Handle sourceHandle, const std::string& sourceName, FUSE::Handle destHandle, const std::string& destName) {
			auto& sourceNode = getNodeInfo(sourceHandle).node;
			auto& destNode = getNodeInfo(destHandle).node;
			
			const auto sourcePath = getPath(sourceHandle) + sourceName;
			const auto destPath = getPath(destHandle) + destName;
			
			if (sourcePath.hasChild(destPath)) {
				// Can't rename to a subdirectory of itself.
				throw FUSE::ErrorException(EINVAL);
			}
			
			if (sourcePath == destPath) {
				// Paths are the same; nothing to do.
				return;
			}
			
			Directory sourceDirectory(sourceNode);
			Directory destDirectory(destNode);
			
			if (!sourceDirectory.hasChild(sourceName)) {
				throw FUSE::ErrorException(ENOENT);
			}
			
			destDirectory.forceAddChild(destName, sourceDirectory.getChild(sourceName));
			sourceDirectory.removeChild(sourceName);
			
			// Re-map any affected metadata.
			removeAllMetadata(destPath);
			moveAllMetadata(sourcePath, destPath);
			
			// Re-map any affected handles.
			detachAllHandles(destPath);
			moveAllHandles(sourcePath, destPath);
		}
		
		struct stat NodeSystem::getAttributes(FUSE::Handle handle) const {
			auto& parentNodeInfo = getNodeInfo(handle);
			return getNodeAttr(parentNodeInfo.metadata, parentNodeInfo.node);
		}
		
	}
	
}

