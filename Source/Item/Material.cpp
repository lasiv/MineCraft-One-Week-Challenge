#include "Material.h"

#define MAX_STACK_SIZE 64

const Material Material::NOTHING(ID::Nothing, 0, false, "None");
const Material Material::GRASS_BLOCK(ID::Grass, MAX_STACK_SIZE, true, "Grass Block");
const Material Material::DIRT_BLOCK(ID::Dirt, MAX_STACK_SIZE, true, "Dirt Block");
const Material Material::STONE_BLOCK(ID::Stone, MAX_STACK_SIZE, true, "Stone Block");
const Material Material::OAK_BARK_BLOCK(ID::OakBark, MAX_STACK_SIZE, true,
                                        "Oak Bark Block");
const Material Material::OAK_LEAF_BLOCK(ID::OakLeaf, MAX_STACK_SIZE, true,
                                        "Oak Leaf Block");
const Material Material::SAND_BLOCK(ID::Sand, MAX_STACK_SIZE, true, "Sand Block");
const Material Material::CACTUS_BLOCK(ID::Cactus, MAX_STACK_SIZE, true, "Cactus Block");

const Material Material::ROSE(ID::Rose, MAX_STACK_SIZE, true, "Rose");
const Material Material::TALL_GRASS(ID::TallGrass, MAX_STACK_SIZE, true, "Tall Grass");
const Material Material::DEAD_SHRUB(ID::DeadShrub, MAX_STACK_SIZE, true, "Dead Shrub");

Material::Material(Material::ID id, int maxStack, bool isBlock,
                   std::string &&name)
    : id(id)
    , maxStackSize(maxStack)
    , isBlock(isBlock)
    , name(std::move(name))
{
}

BlockId Material::toBlockID() const
{
    switch (id) {
        case Nothing:
            return BlockId::Air;

        case Grass:
            return BlockId::Grass;

        case Dirt:
            return BlockId::Dirt;

        case Stone:
            return BlockId::Stone;

        case OakBark:
            return BlockId::OakBark;

        case OakLeaf:
            return BlockId::OakLeaf;

        case Sand:
            return BlockId::Sand;

        case Cactus:
            return BlockId::Cactus;

        case TallGrass:
            return BlockId::TallGrass;

        case Rose:
            return BlockId::Rose;

        case DeadShrub:
            return BlockId::DeadShrub;

        default:
            return BlockId::NUM_TYPES;
    }
}

const Material &Material::toMaterial(BlockId id)
{
    switch (id) {
        case BlockId::Grass:
            return GRASS_BLOCK;

        case BlockId::Dirt:
            return DIRT_BLOCK;

        case BlockId::Stone:
            return STONE_BLOCK;

        case BlockId::OakBark:
            return OAK_BARK_BLOCK;

        case BlockId::OakLeaf:
            return OAK_LEAF_BLOCK;

        case BlockId::Sand:
            return SAND_BLOCK;

        case BlockId::Cactus:
            return CACTUS_BLOCK;

        case BlockId::Rose:
            return ROSE;

        case BlockId::TallGrass:
            return TALL_GRASS;

        case BlockId::DeadShrub:
            return DEAD_SHRUB;

        default:
            return NOTHING;
    }
}
