#ifndef CHUNKBLOCK_H_INCLUDED
#define CHUNKBLOCK_H_INCLUDED

#include "BlockId.h"

struct BlockDataHolder;
class BlockType;

/**
 * @struct ChunkBlock
 * @brief Represents a block in the chunk.
 * 
 * @details
 * The ChunkBlock class is a lightweight representation of a block in the chunk. It holds
 * the block ID and provides methods to access the block data and type.
 */
struct ChunkBlock {
    ChunkBlock() = default;

    /**
     * @brief Constructor for ChunkBlock.
     * 
     * @param id The block ID.
     * 
     * @details
     * This constructor initializes the ChunkBlock with the given block ID.
     * The ID is expected to be a valid Block_t value.
     */
    ChunkBlock(Block_t id);

    /**
     * @brief Constructor for ChunkBlock.
     * 
     * @param id The block ID as a BlockId enum.
     * 
     * @details
     * This constructor initializes the ChunkBlock with the given block ID.
     * The ID is expected to be a valid BlockId value.
     * The BlockId is converted to Block_t for internal representation.
     */
    ChunkBlock(BlockId id);

    const BlockDataHolder &getData() const;
    const BlockType &getType() const;

    bool operator==(ChunkBlock other) const
    {
        return id == other.id;
    }

    bool operator!=(ChunkBlock other) const
    {
        return id != other.id;
    }

    Block_t id = 0;
};

#endif // CHUNKBLOCK_H_INCLUDED
