#ifndef MATERIAL_H_INCLUDED
#define MATERIAL_H_INCLUDED

#include "../World/Block/BlockId.h"
#include <string>

#include "../Util/NonCopyable.h"

/**
 * @struct Material
 * @brief Defines shared data for a material type used in the game.
 * 
 * @details
 * The Material struct represents the canonical definition of a specific material or block type,
 * such as Dirt, Stone, or OakLeaf. Each material instance stores shared, read-only properties
 * like name, block ID, max stack size, and whether it is a block.
 * 
 * Only one instance of each material exists, exposed as `const static` members (e.g., GRASS_BLOCK),
 * allowing the game to reference materials by pointer or enum ID (`Material::ID`) without creating copies.
 */
struct Material : public NonCopyable {
    enum ID {
        Nothing,
        Grass,
        Dirt,
        Stone,
        OakBark,
        OakLeaf,
        Sand,
        Cactus,
        Rose,
        TallGrass,
        DeadShrub
    };

    const static Material NOTHING, GRASS_BLOCK, DIRT_BLOCK, STONE_BLOCK,
        OAK_BARK_BLOCK, OAK_LEAF_BLOCK, SAND_BLOCK, CACTUS_BLOCK, ROSE,
        TALL_GRASS, DEAD_SHRUB;

    Material(Material::ID id, int maxStack, bool isBlock, std::string &&name);

    /**
     * @brief Converts the material to a BlockId.
     * 
     * @return BlockId The BlockId corresponding to this material.
     * 
     * @details
     * This method converts the material to a BlockId, which is used in the game world
     * to represent the block type. The BlockId is an enumeration that defines all
     * possible block types in the game.
     */
    BlockId toBlockID() const;

    static const Material &toMaterial(BlockId id);

    const Material::ID id;
    const int maxStackSize;
    const bool isBlock;
    const std::string name;
};

namespace std {
template <> struct hash<Material::ID> {
    size_t operator()(const Material::ID &id) const
    {
        std::hash<Material::ID> hasher;

        return hasher(id);
    }
};
} // namespace std

#endif // MATERIAL_H_INCLUDED
