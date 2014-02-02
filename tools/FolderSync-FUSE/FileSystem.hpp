#ifndef OPENP2P_FOLDERSYNC_FUSE_FILESYSTEM_HPP
#define OPENP2P_FOLDERSYNC_FUSE_FILESYSTEM_HPP

#include <stdint.h>

#include <memory>
#include <string>
#include <vector>

#include "Path.hpp"

namespace FUSE {

	class ErrorException {
		public:
			inline ErrorException(int e) :
				error_(e) { }
			
			inline int error() const {
				return error_;
			}
			
		private:
			int error_;
		
	};
	
	class OpenedDirectory {
		public:
			inline virtual ~OpenedDirectory() { }
			
			/**
			 * \brief Read child node names.
			 */
			virtual std::vector<std::string> readNames() const = 0;
			
			/**
			 * \brief Add node.
			 */
			virtual void addNode(bool isDirectory, const std::string& name) = 0;
			
			/**
			 * \brief Remove node.
			 */
			virtual void removeNode(const std::string& name) = 0;
			
	};
	
	class OpenedFile {
		public:
			inline virtual ~OpenedFile() { }
			
			virtual size_t read(size_t offset, uint8_t* buffer, size_t size) const = 0;
			
			virtual size_t write(size_t offset, const uint8_t* buffer, size_t size) = 0;
			
			virtual void resize(size_t size) = 0;
			
	};
	
	class FileSystem {
		public:
			inline virtual ~FileSystem() { }
			
			/**
			 * \brief Open a file.
			 */
			virtual std::unique_ptr<OpenedFile> openFile(const Path& path) = 0;
			
			/**
			 * \brief Open a directory.
			 */
			virtual std::unique_ptr<OpenedDirectory> openDirectory(const Path& path) = 0;
			
			/**
			 * \brief Rename a node.
			 *
			 * This should atomically replace the node at 'destPath'
			 * with the node at 'sourcePath', deleting any node that
			 * currently exists at 'destPath'.
			 */
			virtual void rename(const Path& sourcePath, const Path& destPath) = 0;
			
			/**
			 * \brief Get node attributes.
			 */
			virtual struct stat getAttributes(const Path& name) const = 0;
			
			/**
			 * \brief Change 'mode' (permissions) for a node.
			 */
			virtual void changeMode(const Path& name, mode_t mode) = 0;
			
			/**
			 * \brief Change owner for a node.
			 */
			virtual void changeOwner(const Path& name, uid_t user, gid_t group) = 0;
			
	};
	
	int run(const std::string& mountPoint, FileSystem& fileSystem);
	
}

#endif
