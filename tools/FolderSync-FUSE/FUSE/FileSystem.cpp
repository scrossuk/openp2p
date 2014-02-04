#define FUSE_USE_VERSION 30

#include <assert.h>
#include <limits.h>
#include <string.h>
#include <time.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

#include <fuse.h>

#include <FUSE/ErrorException.hpp>
#include <FUSE/FileSystem.hpp>
#include <FUSE/Path.hpp>

namespace FUSE {

	namespace {
	
		struct Context {
			std::ofstream log;
			FileSystem& fileSystem;
			
			inline Context(const std::string& logFile, FileSystem& fs)
				: log(logFile.c_str()), fileSystem(fs) { }
		};
		
		Context& ctx() {
			return *((Context*) fuse_get_context()->private_data);
		}
		
		int fs_create(const char* path, mode_t mode, struct fuse_file_info* info) {
			try {
				std::cout << "----------FUSE: " << "create " << path << std::endl;
				
				const Path fullPath(path);
				if (fullPath.empty()) {
					return -EINVAL;
				}
				
				info->fh = ctx().fileSystem.createAndOpenFile(fullPath, mode);
				
				std::cout << "----------FUSE: " << "created file " << path << " has handle " << info->fh << std::endl;
				
				return 0;
			} catch (const ErrorException& e) {
				return -(e.error());
			} catch (const std::exception& e) {
				std::cout << "----------FUSE: " << "ERROR: Terminated with exception: " << e.what() << std::endl;
				return -EIO;
			}
		}
		
		int fs_unlink(const char* path) {
			try {
				std::cout << "----------FUSE: " << "unlink " << path << std::endl;
				
				const Path fullPath(path);
				if (fullPath.empty()) {
					return -EINVAL;
				}
				
				ctx().fileSystem.removeFile(fullPath);
				return 0;
			} catch (const ErrorException& e) {
				return -(e.error());
			} catch (const std::exception& e) {
				std::cout << "----------FUSE: " << "ERROR: Terminated with exception: " << e.what() << std::endl;
				return -EIO;
			}
		}
		
		int fs_rename(const char* src, const char* dst) {
			try {
				std::cout << "----------FUSE: " << "rename " << src << " to " << dst << std::endl;
				ctx().fileSystem.rename(Path(src), Path(dst));
				return 0;
			} catch (const ErrorException& e) {
				return -(e.error());
			} catch (const std::exception& e) {
				std::cout << "----------FUSE: " << "ERROR: Terminated with exception: " << e.what() << std::endl;
				return -EIO;
			}
		}
		
		int fs_open(const char* path, struct fuse_file_info* info) {
			try {
				std::cout << "----------FUSE: " << "open " << path << std::endl;
				info->fh = ctx().fileSystem.openFile(Path(path));
				std::cout << "----------FUSE: " << "opened file " << path << " has handle " << info->fh << std::endl;
				return 0;
			} catch (const ErrorException& e) {
				return -(e.error());
			} catch (const std::exception& e) {
				std::cout << "----------FUSE: " << "ERROR: Terminated with exception: " << e.what() << std::endl;
				return -EIO;
			}
		}
		
		int fs_release(const char*, struct fuse_file_info* info) {
			try {
				std::cout << "----------FUSE: " << "release " << info->fh << std::endl;
				
				ctx().fileSystem.closeFile(info->fh);
				return 0;
			} catch (const ErrorException& e) {
				return -(e.error());
			} catch (const std::exception& e) {
				std::cout << "----------FUSE: " << "ERROR: Terminated with exception: " << e.what() << std::endl;
				return -EIO;
			}
		}
		
		int fs_read(const char*, char* buffer, size_t size, off_t offset, struct fuse_file_info* info) {
			try {
				// std::cout << "----------FUSE: " << "Reading " << size << " bytes from file " << info->fh << "." << std::endl;
				
				if (offset < 0) {
					std::cout << "----------FUSE: " << "ERROR: Invalid offset." << std::endl;
					return -EINVAL;
				}
				
				if (size > INT_MAX) {
					std::cout << "----------FUSE: " << "ERROR: Invalid size (greater than INT_MAX)." << std::endl;
					return -EINVAL;
				}
				
				const size_t result = ctx().fileSystem.readFile(info->fh, static_cast<size_t>(offset), reinterpret_cast<uint8_t*>(buffer), size);
				assert(result <= size);
				return static_cast<int>(result);
			} catch (const ErrorException& e) {
				return -(e.error());
			} catch (const std::exception& e) {
				std::cout << "----------FUSE: " << "ERROR: Terminated with exception: " << e.what() << std::endl;
				return -EIO;
			}
		}
		
		int fs_write(const char*, const char* buffer, size_t size, off_t offset, struct fuse_file_info* info) {
			try {
				// std::cout << "----------FUSE: " << "Writing " << size << " bytes to file " << info->fh << "." << std::endl;
				
				if (offset < 0) {
					std::cout << "----------FUSE: " << "ERROR: Invalid offset." << std::endl;
					return -EINVAL;
				}
				
				if (size > INT_MAX) {
					std::cout << "----------FUSE: " << "ERROR: Invalid size (greater than INT_MAX)." << std::endl;
					return -EINVAL;
				}
				
				const size_t result = ctx().fileSystem.writeFile(info->fh, static_cast<size_t>(offset), reinterpret_cast<const uint8_t*>(buffer), size);
				assert(result <= size);
				return static_cast<int>(result);
			} catch (const ErrorException& e) {
				return -(e.error());
			} catch (const std::exception& e) {
				std::cout << "----------FUSE: " << "ERROR: Terminated with exception: " << e.what() << std::endl;
				return -EIO;
			}
		}
		
		int fs_getattr(const char* path, struct stat* s) {
			try {
				std::cout << "----------FUSE: " << "getattr " << path << std::endl;
				*s = ctx().fileSystem.getAttributes(Path(path));
				return 0;
			} catch (const ErrorException& e) {
				return -(e.error());
			} catch (const std::exception& e) {
				std::cout << "----------FUSE: " << "ERROR: Terminated with exception: " << e.what() << std::endl;
				return -EIO;
			}
		}
		
		int fs_truncate(const char* path, off_t size) {
			try {
				std::cout << "----------FUSE: " << "truncate " << path << " to size " << size << std::endl;
				
				if (size < 0) {
					return -EINVAL;
				}
				
				ctx().fileSystem.resize(Path(path), size);
				return 0;
			} catch (const ErrorException& e) {
				return -(e.error());
			} catch (const std::exception& e) {
				std::cout << "----------FUSE: " << "ERROR: Terminated with exception: " << e.what() << std::endl;
				return -EIO;
			}
		}
		
		int fs_chmod(const char* path, mode_t mode) {
			try {
				std::cout << "----------FUSE: " << "chmod " << path << std::endl;
				ctx().fileSystem.changeMode(Path(path), mode);
				return 0;
			} catch (const ErrorException& e) {
				return -(e.error());
			}
		}
		
		int fs_chown(const char* path, uid_t uid, gid_t gid) {
			try {
				std::cout << "----------FUSE: " << "chown " << path << std::endl;
				ctx().fileSystem.changeOwner(Path(path), uid, gid);
				return 0;
			} catch (const ErrorException& e) {
				return -(e.error());
			} catch (const std::exception& e) {
				std::cout << "----------FUSE: " << "ERROR: Terminated with exception: " << e.what() << std::endl;
				return -EIO;
			}
		}
		
		int fs_mkdir(const char* path, mode_t mode) {
			try {
				std::cout << "----------FUSE: " << "mkdir " << path << std::endl;
				
				const Path fullPath(path);
				if (fullPath.empty()) {
					return -EINVAL;
				}
				
				ctx().fileSystem.createDirectory(fullPath, mode);
				return 0;
			} catch (const ErrorException& e) {
				return -(e.error());
			} catch (const std::exception& e) {
				std::cout << "----------FUSE: " << "ERROR: Terminated with exception: " << e.what() << std::endl;
				return -EIO;
			}
		}
		
		int fs_rmdir(const char* path) {
			try {
				std::cout << "----------FUSE: " << "rmdir " << path << std::endl;
				
				const Path fullPath(path);
				if (fullPath.empty()) {
					return -EINVAL;
				}
				
				ctx().fileSystem.removeDirectory(fullPath);
				return 0;
			} catch (const ErrorException& e) {
				return -(e.error());
			} catch (const std::exception& e) {
				std::cout << "----------FUSE: " << "ERROR: Terminated with exception: " << e.what() << std::endl;
				return -EIO;
			}
		}
		
		int fs_readdir(const char* path, void* buffer, fuse_fill_dir_t filler, off_t, struct fuse_file_info*) {
			try {
				std::cout << "----------FUSE: " << "readdir " << path << std::endl;
				
				// TODO: make sure this gets closed.
				const auto handle = ctx().fileSystem.openDirectory(Path(path));
				
				const auto nodes = ctx().fileSystem.readDirectory(handle);
				
				ctx().fileSystem.closeDirectory(handle);
				
				filler(buffer, ".", 0, 0);
				filler(buffer, "..", 0, 0);
				
				for (auto node: nodes) {
					filler(buffer, node.c_str(), 0, 0);
				}
				
				return 0;
			} catch (const ErrorException& e) {
				return -(e.error());
			} catch (const std::exception& e) {
				std::cout << "----------FUSE: " << "ERROR: Terminated with exception: " << e.what() << std::endl;
				return -EIO;
			}
		}
		
		void* fs_init(struct fuse_conn_info* conn) {
			std::cout << "----------FUSE: " << "Init!" << std::endl;
			conn->max_write = 65536;
			conn->max_readahead = 65536;
			return fuse_get_context()->private_data;
		}
		
		struct fuse_operations fsFuseOperations() {
			struct fuse_operations operations;
			memset(&operations, 0, sizeof(operations));
			
			operations.init = fs_init;
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
			return operations;
		}
		
	}
	
	int run(const std::string& mountPoint, FileSystem& fileSystem) {
		std::vector<const char*> args;
		args.push_back("runFUSE");
		args.push_back("-s");
		// args.push_back("-f");
		args.push_back(mountPoint.c_str());
		
		const auto operations = fsFuseOperations();
		
		Context context("fuse-log.txt", fileSystem);
		
		return fuse_main(args.size(), (char**) args.data(), &operations, &context);
	}
	
}

