#define FUSE_USE_VERSION 30

#include <climits>
#include <cstdarg>
#include <cstring>
#include <ctime>

#include <fstream>
#include <map>
#include <set>
#include <string>

#include <fuse.h>

typedef std::pair<std::string, std::string> PathComponentPair;

PathComponentPair splitPath(const std::string& path) {
	const size_t pos = path.find_last_of('/');
	
	if (pos == std::string::npos) {
		// No forward slashes were found, so the
		// entire path is the second component.
		return PathComponentPair("", path);
	}
	
	const auto directory = path.substr(0, pos);
	const auto file = path.substr(pos + 1, std::string::npos);
	return PathComponentPair(directory, file);
}

PathComponentPair splitFirstDirectory(const std::string& path) {
	const size_t pos = path.find_first_of('/');
	
	if (pos == std::string::npos) {
		// No forward slashes were found, so the
		// entire path is the second component.
		return PathComponentPair("", path);
	}
	
	const auto directory = path.substr(0, pos);
	const auto file = path.substr(pos + 1, std::string::npos);
	return PathComponentPair(directory, file);
}

std::string stripSlash(const std::string& path) {
	if (path.empty() || (path[0] != '/')) {
		// This function expects a forward slash
		// at the start of the path.
		throw ENOENT;
	}
	
	return path.substr(1, std::string::npos);
}

class Node {
	public:
		Node(mode_t mode) : m_mode(mode), m_atime(time(0)), m_mtime(time(0)), m_uid(0), m_gid(0) { }
		
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
		
		void Chmod(mode_t mode) {
			m_mode = mode;
		}
		
		void Chown(uid_t uid, gid_t gid) {
			m_uid = uid;
			m_gid = gid;
		}
		
		virtual int read(char*, size_t, off_t) {
			throw EISDIR;
		}
		
		virtual int write(const char*, size_t, off_t) {
			throw EISDIR;
		}
		
		virtual void truncate(size_t) {
			throw EISDIR;
		}
		
		virtual std::map<std::string, Node*> readDir() {
			throw ENOTDIR;
		}
		
		virtual Node* getNode(const std::string&) {
			throw ENOTDIR;
		}
		
		virtual void addNode(const std::string&, Node*) {
			throw ENOTDIR;
		}
		
		virtual void removeNode(const std::string&) {
			throw ENOTDIR;
		}
		
		virtual void ReadLink(char*, size_t) {
			throw ERANGE;
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
		
		virtual void getSubclassAttr(struct stat& s) = 0;
		
		mode_t m_mode;
		time_t m_atime;
		time_t m_mtime;
		uid_t m_uid;
		gid_t m_gid;
};

class File : public Node {
	public:
		File(mode_t mode) : Node(mode), m_size(0), m_blocksize(4096) {
		}
		
		~File() { }
		
		int read(char* buffer, size_t size, off_t offset) {
			if ((size > INT_MAX) || (offset < 0) || ((size_t)offset > m_size)) {
				throw EINVAL;
			}
			
			updateTime(false);
			
			const size_t readSize = std::min<size_t>(size, m_size - offset);
			memset(buffer, 0, readSize);
			return readSize;
		}
		
		int write(const char* buffer, size_t size, off_t offset) {
			updateTime(true);
			
			(void) buffer;
			
			// Make sure the file is big enough.
			m_size = std::max<size_t>(m_size, size + offset);
			
			return size;
		}
		
		void truncate(size_t size) {
			updateTime(true);
			
			m_size = size;
		}
		
	private:
		void getSubclassAttr(struct stat& s) {
			s.st_size = m_size;
			s.st_nlink = 1;
			s.st_mode |= S_IFREG;
			s.st_blksize = m_blocksize;
		}
		
		std::map<size_t, char*> m_blocks;
		size_t m_size;
		const size_t m_blocksize;
};

class Directory : public Node {
	public:
		Directory(mode_t mode) : Node(mode) {
		}
		
		~Directory() {
			for (auto node : m_nodes) {
				delete node.second;
			}
		}
		
		std::map<std::string, Node*> readDir() {
			updateTime(false);
			return m_nodes;
		}
		
		Node* getNode(const std::string& path) {
			const auto strippedPath = stripSlash(path);
			
			// No more path; this is what we want.
			if (strippedPath.empty()) {
				return this;
			}
			
			updateTime(false);
			
			const auto components = splitFirstDirectory(strippedPath);
			
			if (components.first.empty()) {
				// No '/', so it's (hopefully) in this directory.
				if (!m_nodes.count(components.second)) {
					// No such file.
					throw ENOENT;
				}
				
				return m_nodes[components.second];
			} else {
				if (m_nodes.count(components.first) == 0) {
					// Directory does not exist.
					throw ENOENT;
				}
				
				return m_nodes[components.first]->getNode(components.second);
			}
		}
		
		void addNode(const std::string& path, Node* node) {
			const auto components = splitPath(path);
			
			if (components.first.empty()) {
				// Adding to self.
				if (m_nodes.count(components.second)) {
					// Node already exists.
					throw EEXIST;
				}
				
				updateTime(true);
				m_nodes[components.second] = node;
			} else {
				Node* parentNode = getNode(components.first);
				parentNode->addNode(components.second, node);
			}
		}
		
		void removeNode(const std::string& path) {
			const auto components = splitPath(path);
			
			if (components.first.empty()) {
				// Removing from self.
				if (m_nodes.count(components.second) == 0) {
					// Node doesn't exist.
					throw ENOENT;
				}
				
				updateTime(true);
				m_nodes.erase(components.second);
			} else {
				auto parentNode = getNode(components.first);
				parentNode->removeNode(components.second);
			}
		}
		
	private:
		void getSubclassAttr(struct stat& s) {
			s.st_nlink = 2;
			s.st_mode |= S_IFDIR;
		}
		
		std::map<std::string, Node*> m_nodes;
		
};

Directory root(0755);

static std::ofstream& logFile() {
	static std::ofstream stream("logFile.txt");
	return stream;
}

int fs_read(const char* path, char* buffer, size_t size, off_t offset, struct fuse_file_info* info) {
	(void) info;
	try {
		logFile() << "read " << path << " at offset " << offset << " with size " << size << std::endl;
		return root.getNode(path)->read(buffer, size, offset);
	} catch (int e) {
		return -e;
	}
}

int fs_write(const char* path, const char* buffer, size_t size, off_t offset, struct fuse_file_info* info) {
	(void) info;
	try {
		logFile() << "write " << path << " at offset " << offset << " with size " << size << std::endl;
		return root.getNode(path)->write(buffer, size, offset);
	} catch (int e) {
		return -e;
	}
}

int fs_readdir(const char* path, void* buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi) {
	(void) offset;
	(void) fi;
	try {
		logFile() << "readdir " << path << std::endl;
		
		auto nodes = root.getNode(path)->readDir();
		filler(buffer, ".", 0, 0);
		filler(buffer, "..", 0, 0);
		
		for (auto node : nodes) {
			filler(buffer, node.first.c_str(), 0, 0);
		}
		
		return 0;
	} catch (int e) {
		return -e;
	}
}

int fs_create(const char* path, mode_t mode, struct fuse_file_info* info) {
	(void) info;
	try {
		logFile() << "create " << path << std::endl;
		
		root.addNode(path, new File(mode));
		return 0;
	} catch (int e) {
		return -e;
	}
}

int fs_getattr(const char* path, struct stat* s) {
	try {
		logFile() << "getattr " << path << std::endl;
		
		*s = root.getNode(path)->GetAttr();
		return 0;
	} catch (int e) {
		return -e;
	}
}

int fs_truncate(const char* path, off_t size) {
	try {
		logFile() << "truncate " << path << " to size " << size << std::endl;
		
		if (size < 0) {
			throw EINVAL;
		}
		
		root.getNode(path)->truncate(size);
		return 0;
	} catch (int e) {
		return -e;
	}
}

int fs_chmod(const char* path, mode_t mode) {
	try {
		root.getNode(path)->Chmod(mode);
		return 0;
	} catch (int e) {
		return -e;
	}
}

int fs_chown(const char* path, uid_t uid, gid_t gid) {
	try {
		root.getNode(path)->Chown(uid, gid);
		return 0;
	} catch (int e) {
		return -e;
	}
}

int fs_rename(const char* src, const char* dst) {
	try {
		logFile() << "rename " << src << " to " << dst << std::endl;
		
		Node* node = root.getNode(src);
		root.removeNode(src);
		root.addNode(dst, node);
		return 0;
	} catch (int e) {
		return -e;
	}
}

int fs_unlink(const char* path) {
	try {
		logFile() << "unlink " << path << std::endl;
		
		Node* node = root.getNode(path);
		root.removeNode(path);
		delete node;
		return 0;
	} catch (int e) {
		return -e;
	}
}

int fs_mkdir(const char* path, mode_t mode) {
	try {
		logFile() << "mkdir " << path << std::endl;
		
		root.addNode(path, new Directory(mode));
		return 0;
	} catch (int e) {
		return -e;
	}
}

int fs_rmdir(const char* path) {
	try {
		logFile() << "rmdir " << path << std::endl;
		
		Node* node = root.getNode(path);
		root.removeNode(path);
		delete node;
		return 0;
	} catch (int e) {
		return -e;
	}
}

int main(int argc, char** argv) {
	// Open the log file before running fuse_main().
	(void) logFile();
	
	struct fuse_operations operations;
	
	memset(&operations, 0, sizeof(operations));
	
	operations.read = fs_read;
	operations.write = fs_write;
	operations.readdir = fs_readdir;
	operations.getattr = fs_getattr;
	operations.create = fs_create;
	operations.truncate = fs_truncate;
	operations.chmod = fs_chmod;
	operations.chown = fs_chown;
	operations.rename = fs_rename;
	operations.unlink = fs_unlink;
	operations.mkdir = fs_mkdir;
	operations.rmdir = fs_rmdir;
	
	return fuse_main(argc, argv, &operations, 0);
}

