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
#include <OpenP2P/FolderSync/MemDatabase.hpp>
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
	
	node.sync();
	
	return node.blockId();
}

struct Metadata {
	time_t accessTime, modifyTime;
	
	inline Metadata()
		: accessTime(time(0)), modifyTime(time(0)) { }
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

class DemoOpenedDirectory: public FUSE::OpenedDirectory {
	public:
		DemoOpenedDirectory(const FUSE::Path& path, FolderSync::Database& database, const FolderSync::BlockId& blockId) 
			: path_(path), database_(database), node_(database, blockId), directory_(node_) { }
		
		std::vector<std::string> read() const {
			return directory_.childNames();
		}
		
	private:
		FUSE::Path path_;
		FolderSync::Database& database_;
		FolderSync::Node node_;
		FolderSync::Directory directory_;
			
};

class FileSystemJournal {
	public:
		virtual void updateRootId(const FUSE::Path& path, const FolderSync::BlockId& newId) = 0;
		
		virtual void updateAccessTime(const FUSE::Path& path) = 0;
		
		virtual void updateModifyTime(const FUSE::Path& path) = 0;
	
};

class DemoOpenedFile: public FUSE::OpenedFile {
	public:
		DemoOpenedFile(FileSystemJournal& journal, const FUSE::Path& path, FolderSync::Database& database, const FolderSync::BlockId& blockId)
			: journal_(journal), path_(path), database_(database),
			node_(database, blockId), hasChanged_(false) { }
		
		~DemoOpenedFile() {
			flush();
		}
		
		void flush() {
			node_.sync();
			
			journal_.updateAccessTime(path_);
			if (hasChanged_) {
				journal_.updateModifyTime(path_);
				hasChanged_ = false;
			}
			
			journal_.updateRootId(path_, node_.blockId());
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
		
	private:
		FileSystemJournal& journal_;
		FUSE::Path path_;
		FolderSync::Database& database_;
		FolderSync::Node node_;
		bool hasChanged_;
			
};

class DemoFileSystem: public FUSE::FileSystem, public FileSystemJournal {
	public:
		DemoFileSystem() {
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
		
		void updateRootId(const FUSE::Path& path, const FolderSync::BlockId& newId) {
			rootId_ = calculateNewId(database_, rootId_, newId, path);
		}
		
		void updateAccessTime(const FUSE::Path& path) {
			metadata_[path].accessTime = time(0);
		}
		
		void updateModifyTime(const FUSE::Path& path) {
			metadata_[path].modifyTime = time(0);
		}
		
		void createFile(const FUSE::Path& path, mode_t) {
			if (path.empty()) {
				// Can't create root.
				throw FUSE::ErrorException(ENOENT);
			}
			
			// Add to parent directory.
			FolderSync::Node parentNode(database_, lookup(path.parent()));
			FolderSync::Directory parentDirectory(parentNode);
			
			if (parentDirectory.hasChild(path.back())) {
				throw FUSE::ErrorException(EEXIST);
			}
			
			parentDirectory.addChild(path.back(), emptyFile_);
			
			parentNode.sync();
			
			updateRootId(path.parent(), parentNode.blockId());
			
			metadata_[path] = Metadata();
			updateModifyTime(path.parent());
		}
		
		std::unique_ptr<FUSE::OpenedFile> openFile(const FUSE::Path& path) {
			updateAccessTime(path);
			return std::unique_ptr<FUSE::OpenedFile>(new DemoOpenedFile(*this, path, database_, lookup(path)));
		}
		
		void unlink(const FUSE::Path& path) {
			if (path.empty()) {
				// Can't delete root.
				throw FUSE::ErrorException(ENOENT);
			}
			
			// Remove from parent directory.
			FolderSync::Node parentNode(database_, lookup(path.parent()));
			
			FolderSync::Directory parentDirectory(parentNode);
			
			if (!parentDirectory.hasChild(path.back())) {
				throw FUSE::ErrorException(ENOENT);
			}
			
			parentDirectory.removeChild(path.back());
			
			parentNode.sync();
			
			updateRootId(path.parent(), parentNode.blockId());
			
			updateModifyTime(path.parent());
		}
		
		void rename(const FUSE::Path& sourcePath, const FUSE::Path& destPath) {
			(void) sourcePath;
			(void) destPath;
			
			// Not implemented.
			throw FUSE::ErrorException(ENOSYS);
		}
		
		struct stat getAttributes(const FUSE::Path& path) const {
			logFile() << "getAttributes " << path.toString() << std::endl;
			FolderSync::Node node(database_, lookup(path));
			return getNodeAttr(metadata_.at(path), node);
		}
		
		void resize(const FUSE::Path& path, size_t size) {
			if (size > FolderSync::NODE_MAX_BYTES) {
				throw FUSE::ErrorException(EFBIG);
			}
			
			FolderSync::Node node(database_, lookup(path));
			
			if (node.type() != FolderSync::TYPE_FILE) {
				throw FUSE::ErrorException(EISDIR);
			}
			
			node.resize(size);
			
			node.sync();
			
			updateRootId(path, node.blockId());
			
			updateModifyTime(path);
		}
		
		void changeMode(const FUSE::Path&, mode_t) {
			// Not implemented.
			throw FUSE::ErrorException(ENOSYS);
		}
		
		void changeOwner(const FUSE::Path&, uid_t, gid_t) {
			// Not implemented.
			throw FUSE::ErrorException(ENOSYS);
		}
		
		void createDirectory(const FUSE::Path& path, mode_t) {
			logFile() << "createDirectory " << path.toString() << std::endl;
			
			if (path.empty()) {
				// Can't create root.
				logFile() << "Tried to create empty directory." << std::endl;
				throw FUSE::ErrorException(EEXIST);
			}
			
			// Add to parent directory.
			FolderSync::Node parentNode(database_, lookup(path.parent()));
			FolderSync::Directory parentDirectory(parentNode);
			
			if (parentDirectory.hasChild(path.back())) {
				throw FUSE::ErrorException(EEXIST);
			}
			
			logFile() << "Adding child '" << path.back() << "'." << std::endl;
			
			parentDirectory.addChild(path.back(), emptyDir_);
			
			parentNode.sync();
			
			updateRootId(path.parent(), parentNode.blockId());
			
			logFile() << "Created directory..." << std::endl;
			
			metadata_[path] = Metadata();
			updateModifyTime(path.parent());
		}
		
		void removeDirectory(const FUSE::Path& path) {
			unlink(path);
		}
		
		std::unique_ptr<FUSE::OpenedDirectory> openDirectory(const FUSE::Path& path) {
			updateAccessTime(path);
			return std::unique_ptr<FUSE::OpenedDirectory>(new DemoOpenedDirectory(path, database_, lookup(path)));
		}
		
	private:
		mutable FolderSync::MemDatabase database_;
		std::unordered_map<FUSE::Path, Metadata> metadata_;
		FolderSync::BlockId emptyDir_, emptyFile_;
		FolderSync::BlockId rootId_;
	
};

int main(int argc, char** argv) {
	if (argc != 2) {
		printf("Usage: %s [mount point]\n", argv[0]);
		return 1;
	}
	
	const std::string mountPoint = argv[1];
	
	// Open the log file before running FUSE.
	(void) logFile();
	
	DemoFileSystem demoFileSystem;
	
	return FUSE::run(mountPoint, demoFileSystem);
}

