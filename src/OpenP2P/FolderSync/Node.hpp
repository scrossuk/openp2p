#ifndef OPENP2P_FOLDERSYNC_NODE_HPP
#define OPENP2P_FOLDERSYNC_NODE_HPP

#include <stdint.h>

#include <OpenP2P/FolderSync/BlockCache.hpp>
#include <OpenP2P/FolderSync/BlockId.hpp>
#include <OpenP2P/FolderSync/NodeBlockStore.hpp>

namespace OpenP2P {
	
	namespace FolderSync {
		
		class Database;
		
		typedef uint64_t NodeSize;
		typedef uint64_t NodeOffset;
		
		enum NodeType {
			TYPE_DIRECTORY = 0,
			TYPE_FILE
		};
		
		BlockId CreateEmptyNode(Database& database, NodeType type);
		
		class Node {
			public:
				Node(Database& database, const BlockId& initialBlockId);
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
				 * \brief Flush node changes.
				 * 
				 * Writes the new node blocks to the database.
				 * This will be called automatically when the
				 * node is destroyed.
				 */
				void flush();
				
				bool hasChanged() const;
				
				void resize(NodeSize size);
				
				size_t read(NodeOffset offset, uint8_t* buffer, size_t bufferSize) const;
				
				size_t write(NodeOffset offset, const uint8_t* buffer, size_t bufferSize);
				
			private:
				// Non-copyable.
				Node(const Node&) = delete;
				Node& operator=(Node) = delete;
				
				bool hasChanged_;
				NodeBlockStore blockStore_;
				BlockCache cache_;
				NodeSize size_;
				NodeType type_;
			
		};
		
	}
	
}

#endif
