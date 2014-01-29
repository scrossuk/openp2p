#include <assert.h>
#include <climits>
#include <cstdarg>
#include <cstring>
#include <ctime>

#include <fstream>
#include <map>
#include <set>
#include <string>

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
	
	return node.blockId();
}

struct stat getNodeAttr(const FolderSync::Node& node) {
	struct stat s;
	memset(&s, 0, sizeof(s));
	
	// Mode = 755.
	s.st_mode = (S_IRUSR | S_IWUSR | S_IXUSR) | (S_IRGRP | S_IXGRP) | (S_IROTH | S_IXOTH);
	
	s.st_uid = geteuid();
	s.st_gid = getegid();
	s.st_atime = time(0);
	s.st_mtime = time(0);
	
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
	
};

class DemoOpenedFile: public FUSE::OpenedFile {
	public:
		DemoOpenedFile(FileSystemJournal& journal, const FUSE::Path& path, FolderSync::Database& database, const FolderSync::BlockId& blockId)
			: journal_(journal), path_(path), database_(database), node_(database, blockId) { }
		
		~DemoOpenedFile() {
			node_.sync();
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
			
			return node_.write(offset, buffer, size);
		}
		
	private:
		FileSystemJournal& journal_;
		FUSE::Path path_;
		FolderSync::Database& database_;
		FolderSync::Node node_;
			
};

FUSE::Path getParentPath(const FUSE::Path& path) {
	assert(!path.empty());
	
	FUSE::Path parent(path);
	parent.pop_back();
	return parent;
}

class DemoFileSystem: public FUSE::FileSystem, public FileSystemJournal {
	public:
		DemoFileSystem() {
			auto emptyNode = FolderSync::Node::Empty(database_, FolderSync::TYPE_DIRECTORY);
			rootId_ = emptyNode.blockId();
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
		
		void createFile(const FUSE::Path& path, mode_t) {
			if (path.empty()) {
				// Can't create root.
				throw FUSE::ErrorException(ENOENT);
			}
			
			auto emptyNode = FolderSync::Node::Empty(database_, FolderSync::TYPE_FILE);
			
			// Add to parent directory.
			FolderSync::Node parentNode(database_, lookup(getParentPath(path)));
			FolderSync::Directory parentDirectory(parentNode);
			
			if (parentDirectory.hasChild(path.back())) {
				throw FUSE::ErrorException(EEXIST);
			}
			
			parentDirectory.addChild(path.back(), emptyNode.blockId());
			
			updateRootId(getParentPath(path), parentNode.blockId());
		}
		
		std::unique_ptr<FUSE::OpenedFile> openFile(const FUSE::Path& path) {
			return std::unique_ptr<FUSE::OpenedFile>(new DemoOpenedFile(*this, path, database_, lookup(path)));
		}
		
		void unlink(const FUSE::Path& path) {
			if (path.empty()) {
				// Can't delete root.
				throw FUSE::ErrorException(ENOENT);
			}
			
			// Remove from parent directory.
			FolderSync::Node parentNode(database_, lookup(getParentPath(path)));
			
			FolderSync::Directory parentDirectory(parentNode);
			
			if (!parentDirectory.hasChild(path.back())) {
				throw FUSE::ErrorException(ENOENT);
			}
			
			parentDirectory.removeChild(path.back());
			
			updateRootId(getParentPath(path), parentNode.blockId());
		}
		
		void rename(const FUSE::Path& sourcePath, const FUSE::Path& destPath) {
			(void) sourcePath;
			(void) destPath;
			
			// Not implemented.
			throw FUSE::ErrorException(ENOSYS);
		}
		
		struct stat getAttributes(const FUSE::Path& path) const {
			logFile() << "getAttributes " << FUSE::PathToString(path) << std::endl;
			FolderSync::Node node(database_, lookup(path));
			return getNodeAttr(node);
		}
		
		void resize(const FUSE::Path& path, size_t size) {
			FolderSync::Node node(database_, lookup(path));
			
			if (node.type() != FolderSync::TYPE_FILE) {
				throw FUSE::ErrorException(EISDIR);
			}
			
			node.resize(size);
			
			updateRootId(path, node.blockId());
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
			logFile() << "createDirectory " << FUSE::PathToString(path) << std::endl;
			
			if (path.empty()) {
				// Can't create root.
				logFile() << "Tried to create empty directory." << std::endl;
				throw FUSE::ErrorException(EEXIST);
			}
			
			auto emptyNode = FolderSync::Node::Empty(database_, FolderSync::TYPE_DIRECTORY);
			
			// Add to parent directory.
			FolderSync::Node parentNode(database_, lookup(getParentPath(path)));
			FolderSync::Directory parentDirectory(parentNode);
			
			if (parentDirectory.hasChild(path.back())) {
				throw FUSE::ErrorException(EEXIST);
			}
			
			parentDirectory.addChild(path.back(), emptyNode.blockId());
			
			updateRootId(getParentPath(path), parentNode.blockId());
			
			logFile() << "Created directory..." << std::endl;
		}
		
		void removeDirectory(const FUSE::Path& path) {
			unlink(path);
		}
		
		std::unique_ptr<FUSE::OpenedDirectory> openDirectory(const FUSE::Path& path) {
			return std::unique_ptr<FUSE::OpenedDirectory>(new DemoOpenedDirectory(path, database_, lookup(path)));
		}
		
	private:
		mutable FolderSync::MemDatabase database_;
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

