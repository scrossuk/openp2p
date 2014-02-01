#define FUSE_USE_VERSION 30

#include <assert.h>
#include <limits.h>
#include <string.h>
#include <time.h>

#include <fstream>
#include <memory>
#include <string>
#include <unordered_map>

#include <fuse.h>

#include "FileSystem.hpp"
#include "Path.hpp"

namespace FUSE {

	namespace {
	
		struct Context {
			std::ofstream log;
			FileSystem& fileSystem;
			uint64_t nextHandle;
			std::unordered_map<uint64_t, OpenedFile*> openedFiles;
			
			inline Context(const std::string& logFile, FileSystem& fs)
				: log(logFile.c_str()), fileSystem(fs), nextHandle(0) { }
		};
		
		Context& ctx() {
			return *((Context*) fuse_get_context()->private_data);
		}
		
		int fs_create(const char* path, mode_t mode, struct fuse_file_info* info) {
			try {
				ctx().log << "create " << path << std::endl;
				
				Path parsedPath(path);
				
				ctx().fileSystem.createFile(parsedPath, mode);
				
				auto openedFile = ctx().fileSystem.openFile(parsedPath);
				info->fh = ctx().nextHandle++;
				
				ctx().log << "created file " << path << " has handle " << info->fh << std::endl;
				
				ctx().openedFiles.emplace(info->fh, openedFile.release());
				
				return 0;
			} catch (const ErrorException& e) {
				return -(e.error());
			}/*  catch (const std::exception& e) {
				ctx().log << "ERROR: Terminated with exception: " << e.what() << std::endl;
				return -EIO;
			} */
		}
		
		int fs_unlink(const char* path) {
			try {
				ctx().log << "unlink " << path << std::endl;
				ctx().fileSystem.unlink(Path(path));
				return 0;
			} catch (const ErrorException& e) {
				return -(e.error());
			}/*  catch (const std::exception& e) {
				ctx().log << "ERROR: Terminated with exception: " << e.what() << std::endl;
				return -EIO;
			} */
		}
		
		int fs_rename(const char* src, const char* dst) {
			try {
				ctx().log << "rename " << src << " to " << dst << std::endl;
				ctx().fileSystem.rename(Path(src), Path(dst));
				return 0;
			} catch (const ErrorException& e) {
				return -(e.error());
			}/*  catch (const std::exception& e) {
				ctx().log << "ERROR: Terminated with exception: " << e.what() << std::endl;
				return -EIO;
			} */
		}
		
		int fs_open(const char* path, struct fuse_file_info* info) {
			try {
				ctx().log << "open " << path << std::endl;
				auto openedFile = ctx().fileSystem.openFile(Path(path));
				info->fh = ctx().nextHandle++;
				
				ctx().log << "opened file " << path << " has handle " << info->fh << std::endl;
				
				ctx().openedFiles.emplace(info->fh, openedFile.release());
				return 0;
			} catch (const ErrorException& e) {
				return -(e.error());
			}/*  catch (const std::exception& e) {
				ctx().log << "ERROR: Terminated with exception: " << e.what() << std::endl;
				return -EIO;
			} */
		}
		
		int fs_release(const char*, struct fuse_file_info* info) {
			try {
				ctx().log << "release " << info->fh << std::endl;
				
				const auto it = ctx().openedFiles.find(info->fh);
				if (it == ctx().openedFiles.end()) {
					ctx().log << "ERROR: Invalid file handle." << std::endl;
					return -ENOENT;
				}
				
				const auto openedFile = it->second;
				ctx().openedFiles.erase(it);
				delete openedFile;
				return 0;
			} catch (const ErrorException& e) {
				return -(e.error());
			}/*  catch (const std::exception& e) {
				ctx().log << "ERROR: Terminated with exception: " << e.what() << std::endl;
				return -EIO;
			} */
		}
		
		int fs_read(const char*, char* buffer, size_t size, off_t offset, struct fuse_file_info* info) {
			try {
				if (offset < 0) {
					ctx().log << "ERROR: Invalid offset." << std::endl;
					return -EINVAL;
				}
				
				if (size > INT_MAX) {
					ctx().log << "ERROR: Invalid size (greater than INT_MAX)." << std::endl;
					return -EINVAL;
				}
				
				const auto it = ctx().openedFiles.find(info->fh);
				if (it == ctx().openedFiles.end()) {
					ctx().log << "ERROR: Invalid file handle." << std::endl;
					return -ENOENT;
				}
				
				const size_t result = (it->second)->read(static_cast<size_t>(offset), reinterpret_cast<uint8_t*>(buffer), size);
				assert(result <= size);
				return static_cast<int>(result);
			} catch (const ErrorException& e) {
				return -(e.error());
			}/*  catch (const std::exception& e) {
				ctx().log << "ERROR: Terminated with exception: " << e.what() << std::endl;
				return -EIO;
			} */
		}
		
		int fs_write(const char*, const char* buffer, size_t size, off_t offset, struct fuse_file_info* info) {
			try {
				if (offset < 0) {
					ctx().log << "ERROR: Invalid offset." << std::endl;
					return -EINVAL;
				}
				
				if (size > INT_MAX) {
					ctx().log << "ERROR: Invalid size (greater than INT_MAX)." << std::endl;
					return -EINVAL;
				}
				
				const auto it = ctx().openedFiles.find(info->fh);
				if (it == ctx().openedFiles.end()) {
					ctx().log << "ERROR: Invalid file handle." << std::endl;
					return -ENOENT;
				}
				
				const size_t result = (it->second)->write(static_cast<size_t>(offset), reinterpret_cast<const uint8_t*>(buffer), size);
				assert(result <= size);
				return static_cast<int>(result);
			} catch (const ErrorException& e) {
				return -(e.error());
			}/*  catch (const std::exception& e) {
				ctx().log << "ERROR: Terminated with exception: " << e.what() << std::endl;
				return -EIO;
			} */
		}
		
		int fs_getattr(const char* path, struct stat* s) {
			try {
				ctx().log << "getattr " << path << std::endl;
				*s = ctx().fileSystem.getAttributes(Path(path));
				return 0;
			} catch (const ErrorException& e) {
				return -(e.error());
			}/*  catch (const std::exception& e) {
				ctx().log << "ERROR: Terminated with exception: " << e.what() << std::endl;
				return -EIO;
			} */
		}
		
		int fs_truncate(const char* path, off_t size) {
			try {
				ctx().log << "truncate " << path << " to size " << size << std::endl;
				
				if (size < 0) {
					return -EINVAL;
				}
				
				ctx().fileSystem.resize(Path(path), static_cast<size_t>(size));
				return 0;
			} catch (const ErrorException& e) {
				return -(e.error());
			}/*  catch (const std::exception& e) {
				ctx().log << "ERROR: Terminated with exception: " << e.what() << std::endl;
				return -EIO;
			} */
		}
		
		int fs_chmod(const char* path, mode_t mode) {
			try {
				ctx().log << "chmod " << path << std::endl;
				ctx().fileSystem.changeMode(Path(path), mode);
				return 0;
			} catch (const ErrorException& e) {
				return -(e.error());
			}
		}
		
		int fs_chown(const char* path, uid_t uid, gid_t gid) {
			try {
				ctx().log << "chown " << path << std::endl;
				ctx().fileSystem.changeOwner(Path(path), uid, gid);
				return 0;
			} catch (const ErrorException& e) {
				return -(e.error());
			}/*  catch (const std::exception& e) {
				ctx().log << "ERROR: Terminated with exception: " << e.what() << std::endl;
				return -EIO;
			} */
		}
		
		int fs_mkdir(const char* path, mode_t mode) {
			try {
				ctx().log << "mkdir " << path << std::endl;
				ctx().fileSystem.createDirectory(Path(path), mode);
				return 0;
			} catch (const ErrorException& e) {
				return -(e.error());
			}/*  catch (const std::exception& e) {
				ctx().log << "ERROR: Terminated with exception: " << e.what() << std::endl;
				return -EIO;
			} */
		}
		
		int fs_rmdir(const char* path) {
			try {
				ctx().log << "rmdir " << path << std::endl;
				ctx().fileSystem.removeDirectory(Path(path));
				return 0;
			} catch (const ErrorException& e) {
				return -(e.error());
			}/*  catch (const std::exception& e) {
				ctx().log << "ERROR: Terminated with exception: " << e.what() << std::endl;
				return -EIO;
			} */
		}
		
		int fs_readdir(const char* path, void* buffer, fuse_fill_dir_t filler, off_t, struct fuse_file_info*) {
			try {
				ctx().log << "readdir " << path << std::endl;
				
				const auto openDirectory = ctx().fileSystem.openDirectory(Path(path));
				
				const auto nodes = openDirectory->read();
				filler(buffer, ".", 0, 0);
				filler(buffer, "..", 0, 0);
				
				for (auto node: nodes) {
					filler(buffer, node.c_str(), 0, 0);
				}
				
				return 0;
			} catch (const ErrorException& e) {
				return -(e.error());
			}/*  catch (const std::exception& e) {
				ctx().log << "ERROR: Terminated with exception: " << e.what() << std::endl;
				return -EIO;
			} */
		}
		
		struct fuse_operations fsFuseOperations() {
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
			return operations;
		}
		
	}
	
	int run(const std::string& mountPoint, FileSystem& fileSystem) {
		constexpr int argc = 4;
		const char* argv[argc];
		argv[0] = "runFUSE";
		argv[1] = "-s";
		argv[2] = "-f";
		argv[3] = mountPoint.c_str();
		
		const auto operations = fsFuseOperations();
		
		Context context("fuse-log.txt", fileSystem);
		
		return fuse_main(argc, (char**) argv, &operations, &context);
	}
	
}

