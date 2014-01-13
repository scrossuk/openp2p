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

static std::ofstream& logFile() {
	static std::ofstream stream("logFile.txt");
	return stream;
}

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

PathComponentPair splitFirstComponent(const std::string& path) {
	const size_t pos = path.find_first_of('/');
	
	if (pos == std::string::npos) {
		// No forward slashes were found, so the
		// entire path is the second component.
		return PathComponentPair(path, "");
	}
	
	const auto directory = path.substr(0, pos);
	const auto file = path.substr(pos + 1, std::string::npos);
	return PathComponentPair(directory, file);
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
		
		void chmod(mode_t mode) {
			m_mode = mode;
		}
		
		void chown(uid_t uid, gid_t gid) {
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
		
		virtual Node* removeNode(const std::string&) {
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
		
		Node* getNode(const std::string& name) {
			logFile() << "=== Getting node '" << name << "'." << std::endl;
			
			const auto it = m_nodes.find(name);
			if (it == m_nodes.end()) {
				// No such file.
				throw ENOENT;
			}
			
			updateTime(false);
			
			return it->second;
		}
		
		void addNode(const std::string& name, Node* node) {
			logFile() << "=== Adding node '" << name << "'." << std::endl;
			
			if (m_nodes.count(name) != 0) {
				// Node already exists.
				logFile() << "=== Node already exists." << std::endl;
				throw EEXIST;
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
				throw ENOENT;
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

Directory root(0755);

Node* lookup_recurse(Node* currentNode, const std::string& remainingPath) {
	// No more path; this is what we want.
	if (remainingPath.empty()) {
		logFile() << "=== Found target node." << std::endl;
		return currentNode;
	}
	
	const auto components = splitFirstComponent(remainingPath);
	
	logFile() << "=== Looking for node '" << components.first << "'; rest of path is '" << components.second << "'." << std::endl;
	
	if (components.second.empty()) {
		// No '/', so it's (hopefully) in this directory.
		return currentNode->getNode(components.first);
	} else {
		// Still looking...
		return lookup_recurse(currentNode->getNode(components.first), components.second);
	}
}

std::pair<Node*, std::string> lookup_parent(Node* rootNode, const std::string& rootPath) {
	if (rootPath.empty() || (rootPath[0] != '/')) {
		// The root path is expected to start with
		// a forward slash.
		throw ENOENT;
	}
	
	const auto strippedPath = rootPath.substr(1, std::string::npos);
	
	const auto components = splitPath(strippedPath);
	return std::make_pair(lookup_recurse(rootNode, components.first), components.second);
}

Node* lookup(Node* rootNode, const std::string& rootPath) {
	if (rootPath.empty() || (rootPath[0] != '/')) {
		// The root path is expected to start with
		// a forward slash.
		throw ENOENT;
	}
	
	const auto strippedPath = rootPath.substr(1, std::string::npos);
	
	return lookup_recurse(rootNode, strippedPath);
}

int fs_create(const char* path, mode_t mode, struct fuse_file_info*) {
	try {
		logFile() << "create " << path << std::endl;
		
		const auto pair = lookup_parent(&root, path);
		(pair.first)->addNode(pair.second, new File(mode));
		
		return 0;
	} catch (int e) {
		return -e;
	}
}

int fs_unlink(const char* path) {
	try {
		logFile() << "unlink " << path << std::endl;
		
		const auto pair = lookup_parent(&root, path);
		const auto node = (pair.first)->removeNode(pair.second);
		delete node;
		
		return 0;
	} catch (int e) {
		return -e;
	}
}

int fs_rename(const char* src, const char* dst) {
	try {
		// Remove any previous file at the destination.
		(void) fs_unlink(dst);
		
		logFile() << "rename " << src << " to " << dst << std::endl;
		
		const auto srcPair = lookup_parent(&root, src);
		const auto node = (srcPair.first)->removeNode(srcPair.second);
		
		const auto dstPair = lookup_parent(&root, dst);
		(dstPair.first)->addNode(dstPair.second, node);
		
		return 0;
	} catch (int e) {
		return -e;
	}
}

int fs_open(const char* path, struct fuse_file_info *) {
	logFile() << "open " << path << std::endl;
	
	// Will throw exception if file doesn't exist.
	(void) lookup(&root, path);
	
	return 0;
}

int fs_release(const char* path, struct fuse_file_info *) {
	logFile() << "release " << path << std::endl;
	return 0;
}

int fs_read(const char* path, char* buffer, size_t size, off_t offset, struct fuse_file_info*) {
	try {
		logFile() << "read " << path << " at offset " << offset << " with size " << size << std::endl;
		return lookup(&root, path)->read(buffer, size, offset);
	} catch (int e) {
		return -e;
	}
}

int fs_write(const char* path, const char* buffer, size_t size, off_t offset, struct fuse_file_info*) {
	try {
		logFile() << "write " << path << " at offset " << offset << " with size " << size << std::endl;
		return lookup(&root, path)->write(buffer, size, offset);
	} catch (int e) {
		return -e;
	}
}

int fs_getattr(const char* path, struct stat* s) {
	try {
		logFile() << "getattr " << path << std::endl;
		
		*s = lookup(&root, path)->GetAttr();
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
		
		lookup(&root, path)->truncate(size);
		return 0;
	} catch (int e) {
		return -e;
	}
}

int fs_chmod(const char* path, mode_t mode) {
	try {
		lookup(&root, path)->chmod(mode);
		return 0;
	} catch (int e) {
		return -e;
	}
}

int fs_chown(const char* path, uid_t uid, gid_t gid) {
	try {
		lookup(&root, path)->chown(uid, gid);
		return 0;
	} catch (int e) {
		return -e;
	}
}

int fs_mkdir(const char* path, mode_t mode) {
	try {
		logFile() << "mkdir " << path << std::endl;
		
		const auto pair = lookup_parent(&root, path);
		(pair.first)->addNode(pair.second, new Directory(mode));
		
		return 0;
	} catch (int e) {
		return -e;
	}
}

int fs_rmdir(const char* path) {
	try {
		logFile() << "rmdir " << path << std::endl;
		
		const auto pair = lookup_parent(&root, path);
		const auto node = (pair.first)->removeNode(pair.second);
		delete node;
		
		return 0;
	} catch (int e) {
		return -e;
	}
}

int fs_readdir(const char* path, void* buffer, fuse_fill_dir_t filler, off_t, struct fuse_file_info*) {
	try {
		logFile() << "readdir " << path << std::endl;
		
		auto nodes = lookup(&root, path)->readDir();
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

int main(int argc, char** argv) {
	// Open the log file before running fuse_main().
	(void) logFile();
	
	struct fuse_operations operations;
	
	memset(&operations, 0, sizeof(operations));
	
	operations.open = fs_open;
	operations.release = fs_release;
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

