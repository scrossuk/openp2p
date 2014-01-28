#ifndef OPENP2P_FOLDERSYNC_NODE_HPP
#define OPENP2P_FOLDERSYNC_NODE_HPP

#include <stdint.h>

#include <OpenP2P/FolderSync/Block.hpp>
#include <OpenP2P/FolderSync/BlockId.hpp>
#include <OpenP2P/FolderSync/Database.hpp>

namespace OpenP2P {
	
	namespace FolderSync {
		
		typedef uint64_t NodeSize;
		
		enum NodeType {
			TYPE_DIRECTORY = 0,
			TYPE_FILE
		};
		
		class Node {
			public:
				Node(Database& database, const BlockId& initialBlockId);
				
				static Node Empty(Database& database, NodeType type);
				
				~Node();
				
				/**
				 * \brief Get the node's block ID.
				 *
				 * If the node hasn't been modified (by a call
				 * to 'write'), this will return the same ID as
				 * passed in the constructor.
				 */
				BlockId blockId() const;
				
				/**
				 * \brief Get node size (in bytes).
				 */
				NodeSize size() const;
				
				/**
				 * \brief Get node type.
				 */
				NodeType type() const;
				
				/**
				 * \brief Sync node changes.
				 * 
				 * Writes the new node blocks to the database.
				 * This will be called automatically when the
				 * node is destroyed.
				 */
				void sync();
				
				void resize(NodeSize size);
				
				size_t read(size_t offset, uint8_t* buffer, size_t bufferSize) const;
				
				size_t write(size_t offset, const uint8_t* buffer, size_t bufferSize);
				
			private:
				Database& database_;
				BlockId nodeBlockId_;
				Block nodeBlock_;
				NodeSize size_;
			
		};
		
	}
	
}

#endif
