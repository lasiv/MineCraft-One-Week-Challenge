#ifndef STRUCTUREBUILDER_H_INCLUDED
#define STRUCTUREBUILDER_H_INCLUDED

#include "../../Block/BlockId.h"
#include <vector>

class Chunk;

/**
 * @class StructureBuilder
 * @brief Builds structures in a chunk.
 * 
 * @details
 * The StructureBuilder class is responsible for building structures in a chunk.
 * It provides methods to create columns, rows, and fill areas with blocks. The
 * class uses a vector to store the blocks that will be added to the chunk. The
 * blocks are defined by their coordinates and block ID. The class provides methods
 * to add blocks to the chunk, create columns and rows of blocks, and fill areas
 * with blocks. The blocks are added to the chunk in a specific order to ensure
 * that the structure is built correctly. The class is designed to be flexible and
 * can be used to create various types of structures, such as buildings, trees,
 * and other objects.
 */
class StructureBuilder {
    struct Block {
        Block(BlockId id, int x, int y, int z)
            : id(id)
            , x(x)
            , y(y)
            , z(z)
        {
        }

        BlockId id;
        int x, y, z;
    };

  public:
    /**
     * @brief builds the structure in the specified chunk.
     * 
     * @param chunk The chunk where the structure will be built.
     * 
     * @details
     * This method builds the structure in the specified chunk by adding
     * the blocks to the chunk. It iterates through the vector of blocks
     * and adds each block to the chunk at the specified coordinates.
     */
    void build(Chunk &chunk);

    /**
     * @brief Creates a column of blocks in the specified chunk.
     * 
     * @param x The x-coordinate of the column.
     * @param z The z-coordinate of the column.
     * @param yStart The starting y-coordinate of the column.
     * @param height The height of the column.
     * @param block The block ID of the column.
     * 
     * @details
     * This method creates a column of blocks in the specified chunk.
     * It adds blocks to the vector of blocks at the specified coordinates.
     * The column is created by iterating from the starting y-coordinate
     * to the height and adding blocks at each y-coordinate. The block
     * ID is used to define the type of block in the column. The method
     * ensures that the blocks are added in the correct order to create
     * a vertical column of blocks.
     */
    void makeColumn(int x, int z, int yStart, int height, BlockId block);
    
    /**
     * @brief Creates a row of blocks in the specified chunk.
     * 
     * @param xStart The starting x-coordinate of the row.
     * @param xEnd The ending x-coordinate of the row.
     * @param y The y-coordinate of the row.
     * @param z The z-coordinate of the row.
     * @param block The block ID of the row.
     * 
     * @details
     * This method creates a row of blocks in the specified chunk.
     * It adds blocks to the vector of blocks at the specified coordinates.
     * The row is created by iterating from the starting x-coordinate
     * to the ending x-coordinate and adding blocks at each x-coordinate.
     * The block ID is used to define the type of block in the row. The method
     * ensures that the blocks are added in the correct order to create
     * a horizontal row of blocks.
     */
    void makeRowX(int xStart, int xEnd, int y, int z, BlockId block);
    
    /**
     * @brief Creates a row of blocks in the specified chunk.
     * 
     * @param zStart The starting z-coordinate of the row.
     * @param zEnd The ending z-coordinate of the row.
     * @param x The x-coordinate of the row.
     * @param y The y-coordinate of the row.
     * @param block The block ID of the row.
     * 
     * @details
     * This method creates a row of blocks in the specified chunk.
     * It adds blocks to the vector of blocks at the specified coordinates.
     * The row is created by iterating from the starting z-coordinate
     * to the ending z-coordinate and adding blocks at each z-coordinate.
     * The block ID is used to define the type of block in the row. The method
     * ensures that the blocks are added in the correct order to create
     * a horizontal row of blocks.
     */
    void makeRowZ(int zStart, int zEnd, int x, int y, BlockId block);

    /**
     * @brief Fills an area with blocks in the specified chunk.
     * 
     * @param y The y-coordinate of the area.
     * @param xStart The starting x-coordinate of the area.
     * @param xEnd The ending x-coordinate of the area.
     * @param zStart The starting z-coordinate of the area.
     * @param zEnd The ending z-coordinate of the area.
     * @param block The block ID to fill the area with.
     * 
     * @details
     * This method fills an area with blocks in the specified chunk.
     * It adds blocks to the vector of blocks at the specified coordinates.
     * The area is filled by iterating from the starting x-coordinate
     * to the ending x-coordinate and from the starting z-coordinate
     * to the ending z-coordinate. The block ID is used to define
     * the type of block in the area. The method ensures that the blocks
     * are added in the correct order to fill the entire area with blocks.
     */
    void fill(int y, int xStart, int xEnd, int zStart, int zEnd, BlockId block);

    /**
     * @brief Adds a block to the specified coordinates in the chunk.
     * 
     * @param x The x-coordinate of the block.
     * @param y The y-coordinate of the block.
     * @param z The z-coordinate of the block.
     * @param block The block ID to add.
     * 
     * @details
     * This method adds a block to the specified coordinates in the chunk.
     * It creates a Block object with the specified coordinates and block ID
     * and adds it to the vector of blocks. The method ensures that the
     * blocks are added in the correct order to create the desired structure.
     */
    void addBlock(int x, int y, int z, BlockId block);

  private:
    std::vector<Block> m_blocks;
};

#endif // STRUCTUREBUILDER_H_INCLUDED
