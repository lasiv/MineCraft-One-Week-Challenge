#ifndef BLOCKDATA_H_INCLUDED
#define BLOCKDATA_H_INCLUDED

#include "../../Util/NonCopyable.h"
#include "BlockId.h"
#include <SFML/Graphics.hpp>

/// @brief Allocates meshes to cubes and non-cube entities.
enum class BlockMeshType {
    Cube = 0,
    X = 1,
};

/// @brief Allocates shader behavior to groups of blocks.
enum class BlockShaderType {
    Chunk = 0,
    Liquid = 1,
    Flora = 2,
};

/// @brief Struct designed to hold geometric and tangibility data for each individual block.
struct BlockDataHolder : public NonCopyable {
    BlockId id = BlockId::Air;
    sf::Vector2i texTopCoord = {0, 0};
    sf::Vector2i texSideCoord = {0, 0};
    sf::Vector2i texBottomCoord = {0, 0};

    BlockMeshType meshType = BlockMeshType::Cube;
    BlockShaderType shaderType = BlockShaderType::Chunk;

    bool isOpaque = false;
    bool isCollidable = false;

    int slip = 100;
    float bounce = 0.f;
};

class BlockData : public NonCopyable {
  public:
    BlockData(const std::string &fileName);

    const BlockDataHolder &getBlockData() const;

  private:
    BlockDataHolder m_data;
};

#endif // BLOCKDATA_H_INCLUDED
