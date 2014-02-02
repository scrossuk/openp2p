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

using namespace OpenP2P;

#include "FileSystem.hpp"
#include "Path.hpp"

std::ofstream& logFile() {
	static std::ofstream stream("logFile.txt");
	return stream;
}

FolderSync::BlockId calculateNewId(FolderSync::Database& database, const FolderSync::BlockId& oldId, const FolderSync::BlockId& endNodeId, const FUSE::Path& path, size_t position = 0) {
	if (position == path.size()) {
		return endNodeId;
	}
	
	const std::string& pathComponent = path.at(position);
	
	FolderSync::Node node(database, oldId);
	
	if (node.type() != FolderSync::TYPE_DIRECTORY) {
		logFile() << "Parent is not a directory!" << std::endl;
		throw FUSE::ErrorException(ENOTDIR);
	}
	
	FolderSync::Directory directory(node);
	
	if (!directory.hasChild(pathComponent)) {
		logFile() << "Expected child '" << pathComponent << "' not found!" << std::endl;
		throw FUSE::ErrorException(ENOENT);
	}
	
	const auto childOldId = directory.getChild(pathComponent);
	const auto childNewId = calculateNewId(database, childOldId, endNodeId, path, position + 1);
	
	directory.updateChild(pathComponent, childNewId);
	
	node.flush();
	
	return node.blockId();
}

struct Metadata {
	time_t accessTime, modifyTime;
	
	inline Metadata()
		: accessTime(time(0)), modifyTime(time(0)) { }
	
	inline void updateAccessTime() {
		accessTime = time(0);
	}
	
	inline void updateModifyTime() {
		modifyTime = time(0);
	}
	
};

struct stat getNodeAttr(const Metadata& metadata, const FolderSync::Node& node) {
	struct stat s;
	memset(&s, 0, sizeof(s));
	
	// Mode = 755.
	s.st_mode = (S_IRUSR | S_IWUSR | S_IXUSR) | (S_IRGRP | S_IXGRP) | (S_IROTH | S_IXOTH);
	
	s.st_uid = geteuid();
	s.st_gid = getegid();
	s.st_atime = metadata.accessTime;
	s.st_mtime = metadata.modifyTime;
	
	switch (node.type()) {
		case FolderSync::TYPE_FILE:
		{
			s.st_size = node.size();
			s.st_nlink = 1;
			s.st_mode |= S_IFREG;
			s.st_blksize = FolderSync::BLOCK_SIZE;
			break;
		}
		case FolderSync::TYPE_DIRECTORY:
		{
			s.st_nlink = 2;
			s.st_mode |= S_IFDIR;
			break;
		}
		default:
			throw FUSE::ErrorException(EINVAL);
	}
	
	return s;
}

class FileSystemJournal {
	public:
		virtual void updateRootId(const FUSE::Path& path, const FolderSync::BlockId& newId) = 0;
		
		virtual Metadata& metadata(const FUSE::Path& path) = 0;
		
		virtual void updateModifyTime(const FUSE::Path& path) = 0;
	
};

class DemoOpenedDirectory: public FUSE::OpenedDirectory {
	public:
		DemoOpenedDirectory(FileSystemJournal& journal, const FUSE::Path& path, FolderSync::Database& database, const FolderSync::BlockId& blockId) 
			: journal_(journal), path_(path),
			database_(database), node_(database, blockId),
			directory_(node_), hasChanged_(false) {
				if (node_.type() != FolderSync::TYPE_DIRECTORY) {
					throw FUSE::ErrorException(ENOTDIR);
				}
			}
		
		~DemoOpenedDirectory() {
			flush();
		}
		
		void flush() {
			node_.flush();
			
			journal_.updateAccessTime(path_);
			if (hasChanged_) {
				journal_.updateModifyTime(path_);
				journal_.updateRootId(path_, node_.blockId());
				hasChanged_ = false;
			}
		}
		
		std::vector<std::string> readNames() const {
			return directory_.childNames();
		}
		
		void addNode(bool isDirectory, const std::string& name) {
			if (directory_.hasChild(name)) {
				throw FUSE::ErrorException(EEXIST);
			}
			
			const auto type = isDirectory ? FolderSync::TYPE_DIRECTORY : FolderSync::TYPE_FILE;
			directory_.addChild(name, FolderSync::CreateEmptyNode(database_, type));
		}
		
		void removeNode(const std::string& name) {
			if (!parentDirectory.hasChild(name)) {
				throw FUSE::ErrorException(ENOENT);
			}
			
			directory_.removeChild(name);
		}
		
		BlockId releaseNode(const std::string& name) {
			if (!directory_.hasChild(name)) {
				throw FUSE::ErrorException(ENOENT);
			}
			
			const auto blockId = directory_.getChild(name);
			directory_.removeChild(name);
			return blockId;
		}
		
	private:
		FileSystemJournal& journal_;
		FUSE::Path path_;
		FolderSync::Database& database_;
		FolderSync::Node node_;
		FolderSync::Directory directory_;
		bool hasChanged_;
		
};

class DemoOpenedFile {
	public:
		DemoOpenedFile(FileSystemJournal& journal, const FUSE::Path& path, FolderSync::Database& database, const FolderSync::BlockId& blockId)
			: journal_(journal), path_(path), database_(database),
			node_(database, blockId), hasChanged_(false), isDetached_(false) {
				if (node_.type() != FolderSync::TYPE_FILE) {
					throw FUSE::ErrorException(EISDIR);
				}
			}
		
		void detach() {
			isDetached_ = true;
		}
		
		void updatePath(const FUSE::Path& newPath) {
			path_ = newPath;
		}
		
		void flush() {
			if (isDetached_) return;
			
			node_.flush();
			
			journal_.metadata(path_).updateAccessTime();
			if (hasChanged_) {
				journal_.updateModifyTime(path_);
				journal_.updateRootId(path_, node_.blockId());
				hasChanged_ = false;
			}
		}
		
		size_t read(size_t offset, uint8_t* buffer, size_t size) const {
			if (offset > node_.size()) {
				throw FUSE::ErrorException(EINVAL);
			}
			
			return node_.read(offset, buffer, size);
		}
		
		size_t write(size_t offset, const uint8_t* buffer, size_t size) {
			if (offset > node_.size()) {
				throw FUSE::ErrorException(EINVAL);
			}
			
			if ((offset + size) > FolderSync::NODE_MAX_BYTES) {
				throw FUSE::ErrorException(EFBIG);
			}
			
			hasChanged_ = true;
			
			return node_.write(offset, buffer, size);
		}
		
		void resize(size_t size) {
			node_.resize(size);
		}
		
	private:
		FileSystemJournal& journal_;
		FUSE::Path path_;
		FolderSync::Database& database_;
		FolderSync::Node node_;
		bool hasChanged_, isDetached_;
			
};

class DemoOpenedFileRef: public FUSE::OpenedFile {
	public:
		DemoOpenedFileRef(ContextRef<DemoOpenedFile> openedFile)
			: openedFile_(std::move(openedFile)) { }
		
		~DemoOpenedFileRef() {
			openedFile_->flush();
		}
		
		size_t read(size_t offset, uint8_t* buffer, size_t size) const {
			return openedFile_->read(offset, buffer, size);
		}
		
		size_t write(size_t offset, const uint8_t* buffer, size_t size) {
			return openedFile_->write(offset, buffer, size);
		}
		
		void resize(size_t size) {
			openedFile_->resize(size);
		}
		
	private:
		ContextRef<DemoOpenedFile> openedFile_;
			
};

class TransactionLog {
	public:
		uint64_t allocate(const FUSE::Path& path);
		
		void commit(uint64_t handle, const BlockId& blockId);
		
		void release(uint64_t handle);
	
};

class DemoFileSystem: public FUSE::FileSystem, public FileSystemJournal {
	public:
		DemoFileSystem(FolderSync::Database& database)
			: database_(database) {
			emptyDir_ = FolderSync::CreateEmptyNode(database_, FolderSync::TYPE_DIRECTORY);
			emptyFile_ = FolderSync::CreateEmptyNode(database_, FolderSync::TYPE_FILE);
			rootId_ = emptyDir_;
			metadata_.emplace(FUSE::Path(), Metadata());
		}
		
		FolderSync::BlockId lookup(const FUSE::Path& path) const {
			FolderSync::BlockId currentId = rootId_;
			
			for (size_t i = 0; i < path.size(); i++) {
				const auto& pathComponent = path.at(i);
				FolderSync::Node node(database_, currentId);
				
				if (node.type() != FolderSync::TYPE_DIRECTORY) {
					logFile() << "LOOKUP: Not a directory!" << std::endl;
					throw FUSE::ErrorException(ENOTDIR);
				}
				
				FolderSync::Directory directory(node);
				
				if (!directory.hasChild(pathComponent)) {
					logFile() << "LOOKUP: Expected child '" << pathComponent << "' not found!" << std::endl;
					throw FUSE::ErrorException(ENOENT);
				}
				
				currentId = directory.getChild(pathComponent);
			}
			
			return currentId;
		}
		
		Metadata& metadata(const FUSE::Path& path) {
			return metadata_[path];
		}
		
		void updateRootId(const FUSE::Path& path, const FolderSync::BlockId& newId) {
			rootId_ = calculateNewId(database_, rootId_, newId, path);
		}
		
		void updatePath(const FUSE::Path& path, const FUSE::Path& newPath) {
			auto existingFile = openedFiles_.find(path);
			if (existingFile.get() != nullptr) {
				existingFile->updatePath(newPath);
				return;
			}
			
			auto existingDirectory = openedDirectories_.find(path);
			if (existingDirectory.get() != nullptr) {
				existingDirectory->updatePath(newPath);
				return;
			}
		}
		
		void detachPath(const FUSE::Path& path) {
			auto existingFile = openedFiles_.find(path);
			if (existingFile.get() != nullptr) {
				existingFile->detach();
				return;
			}
			
			auto existingDirectory = openedDirectories_.find(path);
			if (existingDirectory.get() != nullptr) {
				existingDirectory->detach();
				return;
			}
		}
		
		ContextRef<DemoOpenedFile> getOpenedFile(const FUSE::Path& path) {
			auto existingFile = openedFiles_.find(path);
			if (existingFile.get() != nullptr) {
				return std::move(existingFile);
			}
			
			return openedFiles_.insert(path, unique_ptr<DemoOpenedFile>(new DemoOpenedFile(*this, path, database_, lookup(path))));
		}
		
		std::unique_ptr<FUSE::OpenedFile> openFile(const FUSE::Path& path) {
			return std::unique_ptr<FUSE::OpenedFile>(new DemoOpenedFileRef(getOpenedFile(path)));
		}
		
		ContextRef<DemoOpenedDirectory> getOpenedDirectory(const FUSE::Path& path) {
			const auto iterator = openedDirectories_.find(path);
			if (iterator != openedDirectories_.end()) {
				return iterator->second;
			}
			
			return std::shared_ptr<DemoOpenedDirectory>(new DemoOpenedDirectory(*this, path, database_, lookup(path)));
		}
		
		std::unique_ptr<FUSE::OpenedDirectory> openDirectory(const FUSE::Path& path) {
			return std::unique_ptr<FUSE::OpenedDirectory>(new DemoOpenedDirectoryRef(getOpenedDirectory(path)));
		}
		
		void rename(const FUSE::Path& sourcePath, const FUSE::Path& destPath) {
			if (sourcePath.empty() || destPath.empty()) {
				// Can't rename root.
				throw FUSE::ErrorException(ENOENT);
			}
			
			auto sourceOpenedDirectory = getOpenedDirectory(sourcePath.parent());
			auto destOpenedDirectory = getOpenedDirectory(destPath.parent());
			
			// TODO: make atomic.
			const auto sourceId = sourceOpenedDirectory->releaseNode(sourcePath.last());
			destOpenedDirectory->updateNode(destPath.last(), sourceId);
			
			updatePath(sourcePath, destPath);
			detachPath(destPath);
			
			/*FolderSync::Node sourceParentNode(database_, lookup(sourcePath.parent()));
			
			if (sourceParentNode.type() != FolderSync::TYPE_DIRECTORY) {
				throw FUSE::ErrorException(ENOTDIR);
			}
			
			FolderSync::Directory sourceDirectory(sourceParentNode);
			
			if (!sourceDirectory.hasChild(sourcePath.back())) {
				throw FUSE::ErrorException(ENOENT);
			}
			
			const auto sourceId = sourceDirectory.getChild(sourcePath.back());
			
			// Remove source from its position.
			sourceDirectory.removeChild(sourcePath.back());
			
			sourceParentNode.flush();
			
			// Update root ID for source changes.
			updateRootId(sourcePath.parent(), sourceParentNode.blockId());
			
			// -------- Update destination.
			
			FolderSync::Node destParentNode(database_, lookup(sourcePath.parent()));
			
			if (destParentNode.type() != FolderSync::TYPE_DIRECTORY) {
				throw FUSE::ErrorException(ENOTDIR);
			}
			
			FolderSync::Directory destDirectory(destParentNode);
			
			// Add source to destination position, removing
			// any existing file at the destination.
			destDirectory.forceAddChild(destPath.back(), sourceId);
			
			destParentNode.flush();
			
			// Update metadata.
			metadata_[destPath] = metadata_.at(sourcePath);
			metadata_.erase(sourcePath);
			
			// Update root ID for destination changes.
			updateRootId(destPath.parent(), destParentNode.blockId());*/
		}
		
		struct stat getAttributes(const FUSE::Path& path) const {
			logFile() << "getAttributes " << path.toString() << std::endl;
			FolderSync::Node node(database_, lookup(path));
			return getNodeAttr(metadata_.at(path), node);
		}
		
		void changeMode(const FUSE::Path&, mode_t) {
			// Not implemented.
			throw FUSE::ErrorException(ENOSYS);
		}
		
		void changeOwner(const FUSE::Path&, uid_t, gid_t) {
			// Not implemented.
			throw FUSE::ErrorException(ENOSYS);
		}
		
	private:
		FolderSync::Database& database_;
		ContextTable<DemoOpenedFile> openedFiles_;
		ContextTable<DemoOpenedDirectory> openedDirectories_;
		std::unordered_map<FUSE::Path, Metadata> metadata_;
		FolderSync::BlockId emptyDir_, emptyFile_;
		FolderSync::BlockId rootId_;
	
};

int main(int argc, char** argv) {
	if (argc != 3) {
		printf("Usage: %s [mount point] [block-path]\n", argv[0]);
		return 1;
	}
	
	printf("Max file size: %.2f GiB.\n",
		double(OpenP2P::FolderSync::NODE_MAX_BYTES) / double(1024 * 1024 * 1024));
	
	const std::string mountPoint = argv[1];
	const std::string blockPath = argv[2];
	
	// Open the log file before running FUSE.
	(void) logFile();
	
	FolderSync::FileDatabase database(blockPath);
	DemoFileSystem demoFileSystem(database);
	
	return FUSE::run(mountPoint, demoFileSystem);
}

