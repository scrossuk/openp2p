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
#include <OpenP2P/FolderSync/MemDatabase.hpp>

using namespace OpenP2P;

#include "FileSystem.hpp"
#include "Path.hpp"

std::ofstream& logFile() {
	static std::ofstream stream("logFile.txt");
	return stream;
}

class Node {
	public:
		Node(mode_t mode) : m_mode(mode), m_atime(time(0)), m_mtime(time(0)), m_uid(geteuid()), m_gid(getegid()) { }
		
		virtual ~Node() { }
		
		struct stat GetAttr() {
			struct stat s;
			memset(&s, 0, sizeof(s));
			
			s.st_mode = m_mode;
			s.st_uid = m_uid;
			s.st_gid = m_gid;
			s.st_atime = m_atime;
			s.st_mtime = m_mtime;
			
			getSubclassAttr(s);
			return s;
		}
		
		void chmod(mode_t mode) {
			m_mode = mode;
		}
		
		void chown(uid_t uid, gid_t gid) {
			m_uid = uid;
			m_gid = gid;
		}
		
		virtual int read(char*, size_t, off_t) {
			throw FUSE::ErrorException(EISDIR);
		}
		
		virtual int write(const char*, size_t, off_t) {
			throw FUSE::ErrorException(EISDIR);
		}
		
		virtual void truncate(size_t) {
			throw FUSE::ErrorException(EISDIR);
		}
		
		virtual std::map<std::string, Node*> readDir() {
			throw FUSE::ErrorException(ENOTDIR);
		}
		
		virtual Node* getNode(const std::string&) {
			throw FUSE::ErrorException(ENOTDIR);
		}
		
		virtual void addNode(const std::string&, Node*) {
			throw FUSE::ErrorException(ENOTDIR);
		}
		
		virtual Node* removeNode(const std::string&) {
			throw FUSE::ErrorException(ENOTDIR);
		}
		
		virtual void ReadLink(char*, size_t) {
			throw FUSE::ErrorException(ERANGE);
		}
		
	protected:
		void updateTime(bool mod) {
			if (mod) {
				m_mtime = time(0);
			} else {
				m_atime = time(0);
			}
		}
		
	private:
		Node(const Node&) = delete;
		Node& operator=(const Node&) = delete;
		
		virtual void getSubclassAttr(struct stat&) = 0;
		
		mode_t m_mode;
		time_t m_atime;
		time_t m_mtime;
		uid_t m_uid;
		gid_t m_gid;
};

FolderSync::Block ZeroBlock() {
	FolderSync::Block zeroBlock;
	zeroBlock.fill(0x00);
	return zeroBlock;
}

class File : public Node {
	public:
		File(FolderSync::Database& database, mode_t mode) : Node(mode),
			database_(database), size_(0) { }
		
		~File() { }
		
		size_t read(size_t offset, uint8_t* buffer, size_t size) {
			if (offset > size_) {
				throw FUSE::ErrorException(EINVAL);
			}
			
			updateTime(false);
			
			const size_t readSize = std::min<size_t>(size, size_ - offset);
			
			for (size_t pos = 0; pos < readSize; ) {
				const size_t blockIndex = (offset + pos) / FolderSync::BLOCK_SIZE;
				const size_t blockPosition = (offset + pos) % FolderSync::BLOCK_SIZE;
				
				const auto block = database_.loadBlock(blockIds_.at(blockIndex));
				
				const size_t blockReadSize = std::min<size_t>(readSize - pos, FolderSync::BLOCK_SIZE - blockPosition);
				memcpy(&buffer[pos], &block[blockPosition], blockReadSize);
				pos += blockReadSize;
			}
			
			return readSize;
		}
		
		size_t write(size_t offset, const uint8_t* buffer, size_t size) {
			if (offset > size_) {
				throw FUSE::ErrorException(EINVAL);
			}
			
			updateTime(true);
			
			// Make sure the file is big enough.
			resize(std::max<size_t>(size_, size + offset));
			
			const size_t writeSize = size;
			
			for (size_t pos = 0; pos < writeSize; ) {
				const size_t blockIndex = (offset + pos) / FolderSync::BLOCK_SIZE;
				const size_t blockPosition = (offset + pos) % FolderSync::BLOCK_SIZE;
				
				auto block = database_.loadBlock(blockIds_.at(blockIndex));
				
				const size_t blockWriteSize = std::min<size_t>(writeSize - pos, FolderSync::BLOCK_SIZE - blockPosition);
				memcpy(&block[blockPosition], &buffer[pos], blockWriteSize);
				
				const auto blockId = FolderSync::BlockId::Generate(block);
				database_.storeBlock(blockId, block);
				blockIds_.at(blockIndex) = blockId;
				
				pos += blockWriteSize;
			}
			
			return writeSize;
		}
		
		void truncate(size_t size) {
			updateTime(true);
			
			resize(size);
		}
		
		void resize(size_t size) {
			const size_t newSize = size;
			const size_t numBlocks = (newSize + (FolderSync::BLOCK_SIZE - 1)) / FolderSync::BLOCK_SIZE;
			
			if (numBlocks > blockIds_.size()) {
				// Add new blocks required.
				const auto zeroBlock = ZeroBlock();
				const auto zeroBlockId = FolderSync::BlockId::Generate(zeroBlock);
				database_.storeBlock(zeroBlockId, zeroBlock);
				
				for (size_t i = blockIds_.size(); i < numBlocks; i++) {
					blockIds_.push_back(zeroBlockId);
				}
			} else {
				// Remove blocks no longer required.
				for (size_t i = numBlocks; i < blockIds_.size(); i++) {
					blockIds_.pop_back();
				}
			}
			
			size_ = newSize;
		}
		
	private:
		void getSubclassAttr(struct stat& s) {
			s.st_size = size_;
			s.st_nlink = 1;
			s.st_mode |= S_IFREG;
			s.st_blksize = FolderSync::BLOCK_SIZE;
		}
		
		FolderSync::Database& database_;
		size_t size_;
		std::vector<FolderSync::BlockId> blockIds_;
		
};

class Directory : public Node {
	public:
		Directory(mode_t mode) : Node(mode) { }
		
		~Directory() {
			for (auto node : m_nodes) {
				delete node.second;
			}
		}
		
		std::map<std::string, Node*> readDir() {
			updateTime(false);
			return m_nodes;
		}
		
		Node* getNode(const std::string& name) {
			logFile() << "=== Getting node '" << name << "'." << std::endl;
			
			const auto it = m_nodes.find(name);
			
			if (it == m_nodes.end()) {
				// No such file.
				throw FUSE::ErrorException(ENOENT);
			}
			
			updateTime(false);
			
			return it->second;
		}
		
		void addNode(const std::string& name, Node* node) {
			logFile() << "=== Adding node '" << name << "'." << std::endl;
			
			if (m_nodes.count(name) != 0) {
				// Node already exists.
				logFile() << "=== Node already exists." << std::endl;
				throw FUSE::ErrorException(EEXIST);
			}
			
			updateTime(true);
			m_nodes[name] = node;
		}
		
		Node* removeNode(const std::string& name) {
			logFile() << "=== Removing node '" << name << "'." << std::endl;
			
			const auto it = m_nodes.find(name);
			
			// Removing from self.
			if (it == m_nodes.end()) {
				// Node doesn't exist.
				logFile() << "=== Node doesn't exist." << std::endl;
				throw FUSE::ErrorException(ENOENT);
			}
			
			updateTime(true);
			
			const auto node = it->second;
			m_nodes.erase(it);
			return node;
		}
		
	private:
		void getSubclassAttr(struct stat& s) {
			s.st_nlink = 2;
			s.st_mode |= S_IFDIR;
		}
		
		std::map<std::string, Node*> m_nodes;
		
};

class DemoOpenedDirectory: public FUSE::OpenedDirectory {
	public:
		DemoOpenedDirectory(Directory& directory) : directory_(directory) { }
		
		std::vector<std::string> read() const {
			std::vector<std::string> children;
			const auto nodes = directory_.readDir();
			for (const auto& it: nodes) {
				children.push_back(it.first);
			}
			return children;
		}
	
	private:
		Directory& directory_;
			
};

class DemoOpenedFile: public FUSE::OpenedFile {
	public:
		DemoOpenedFile(File& file)
			: file_(file) { }
		
		size_t read(size_t offset, uint8_t* buffer, size_t size) const {
			return file_.read(offset, buffer, size);
		}
		
		size_t write(size_t offset, const uint8_t* buffer, size_t size) {
			return file_.write(offset, buffer, size);
		}
		
	private:
		File& file_;
			
};

FUSE::Path getParentPath(const FUSE::Path& path) {
	assert(!path.empty());
	
	FUSE::Path parent(path);
	parent.pop_back();
	return parent;
}

class DemoFileSystem: public FUSE::FileSystem {
	public:
		DemoFileSystem() : root_(0755) { }
		
		Node* lookup(const FUSE::Path& path) const {
			Node* node = (Node*) &root_;
			for (size_t i = 0; i < path.size(); i++) {
				node = node->getNode(path.at(i));
			}
			return node;
		}
		
		std::unique_ptr<FUSE::OpenedFile> createFile(const FUSE::Path& path, mode_t mode) {
			if (path.empty()) {
				// Can't create root.
				throw FUSE::ErrorException(ENOENT);
			}
			
			const auto parentPath = getParentPath(path);
			const auto parentNode = lookup(parentPath);
			
			File* node = new File(database_, mode);
			parentNode->addNode(path.back(), node);
			
			return std::unique_ptr<FUSE::OpenedFile>(new DemoOpenedFile(*node));
		}
		
		std::unique_ptr<FUSE::OpenedFile> openFile(const FUSE::Path& path) {
			// TODO: remove this cast!
			File* file = dynamic_cast<File*>(lookup(path));
			
			if (file == NULL) {
				throw FUSE::ErrorException(EISDIR);
			}
			
			return std::unique_ptr<FUSE::OpenedFile>(new DemoOpenedFile(*file));
		}
		
		void unlink(const FUSE::Path& path) {
			if (path.empty()) {
				// Can't delete root.
				throw FUSE::ErrorException(ENOENT);
			}
			
			const auto parentPath = getParentPath(path);
			const auto parentNode = lookup(parentPath);
			const auto node = parentNode->removeNode(path.back());
			delete node;
		}
		
		void rename(const FUSE::Path& sourcePath, const FUSE::Path& destPath) {
			// Remove any existing file at destination.
			try {
				unlink(destPath);
			} catch (const FUSE::ErrorException&) { }
			
			const auto sourceParentPath = getParentPath(sourcePath);
			const auto sourceParentNode = lookup(sourceParentPath);
			const auto node = sourceParentNode->removeNode(sourcePath.back());
			
			const auto destParentPath = getParentPath(destPath);
			const auto destParentNode = lookup(destParentPath);
			destParentNode->addNode(destPath.back(), node);
		}
		
		struct stat getAttributes(const FUSE::Path& path) const {
			return lookup(path)->GetAttr();
		}
		
		void resize(const FUSE::Path& path, size_t size) {
			lookup(path)->truncate(size);
		}
		
		void changeMode(const FUSE::Path& path, mode_t mode) {
			lookup(path)->chmod(mode);
		}
		
		void changeOwner(const FUSE::Path& path, uid_t user, gid_t group) {
			lookup(path)->chown(user, group);
		}
		
		void createDirectory(const FUSE::Path& path, mode_t mode) {
			if (path.empty()) {
				// Can't create root.
				throw FUSE::ErrorException(ENOENT);
			}
			
			const auto parentPath = getParentPath(path);
			const auto parentNode = lookup(parentPath);
			parentNode->addNode(path.back(), new Directory(mode));
		}
		
		void removeDirectory(const FUSE::Path& path) {
			if (path.empty()) {
				// Can't delete root.
				throw FUSE::ErrorException(ENOENT);
			}
			
			const auto parentPath = getParentPath(path);
			const auto parentNode = lookup(parentPath);
			const auto node = parentNode->removeNode(path.back());
			delete node;
		}
		
		std::unique_ptr<FUSE::OpenedDirectory> openDirectory(const FUSE::Path& path) {
			// TODO: remove this cast!
			Directory* dir = dynamic_cast<Directory*>(lookup(path));
			
			if (dir == NULL) {
				throw FUSE::ErrorException(ENOTDIR);
			}
			
			return std::unique_ptr<FUSE::OpenedDirectory>(new DemoOpenedDirectory(*dir));
		}
		
	private:
		FolderSync::MemDatabase database_;
		Directory root_;
	
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

