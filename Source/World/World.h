#ifndef WORLD_H_INCLUDED
#define WORLD_H_INCLUDED

#include <atomic>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include "../Util/NonCopyable.h"
#include "Chunk/Chunk.h"
#include "Chunk/ChunkManager.h"

#include "Event/IWorldEvent.h"

#include "../Config.h"

class RenderMaster;
class Camera;
class Player;

struct Entity;

/**
 * @class World
 * @brief Represents the game world, managing chunks and blocks.
 * 
 * @details
 * The World class is responsible for managing the game world, including loading and
 * unloading chunks, handling block interactions, and rendering the world. It provides
 * methods to get and set blocks, update the world state, and render the world using
 * a RenderMaster object.
 */
class World : public NonCopyable {
  public:
    /**
     * @brief Constructor for the World class.
     * 
     * @param camera The camera object used for rendering the world.
     * @param config The configuration object containing world settings.
     * @param player The player object representing the player in the world.
     * 
     * @details
     * This constructor initializes the World object, setting up the chunk manager and
     * spawning the player at a random location within the world. It also starts a thread
     * to load chunks around the player.
     */
    World(const Camera &camera, const Config &config, Player &player);
    /**
     * @brief Destructor for the World class.
     * 
     * @details
     * This destructor is responsible for cleaning up the resources used by the World class.
     * It stops the chunk loading threads and waits for them to finish before exiting.
     */
    ~World();

    /**
     * @brief Gets the block at the specified coordinates.
     * 
     * @param x The x-coordinate of the block.
     * @param y The y-coordinate of the block.
     * @param z The z-coordinate of the block.
     * 
     * @return The ChunkBlock at the specified coordinates.
     * 
     * @details
     * This function retrieves the block at the specified world coordinates. It calculates
     * the chunk position and block position within the chunk, and returns the corresponding
     * ChunkBlock. The coordinates are expected to be in world space.
     */
    ChunkBlock getBlock(int x, int y, int z);

    
    void setBlock(int x, int y, int z, ChunkBlock block);

    /**
     * @brief Updates the world state.
     * 
     * @param camera The camera object used to determine the current view.
     * 
     * @details
     * This function handles the world update logic, including processing events and
     * updating the chunk meshes. It also handles a toggle key to reset the chunk meshes
     * and load distance.
     */
    void update(const Camera &camera);

    /**
     * 
     */
    void updateChunk(int blockX, int blockY, int blockZ);

    void renderWorld(RenderMaster &master, const Camera &camera);

    ChunkManager &getChunkManager();

    static VectorXZ getBlockXZ(int x, int z);
    static VectorXZ getChunkXZ(int x, int z);

    // void collisionTest(Entity &entity);

    template <typename T, typename... Args> void addEvent(Args &&... args)
    {
        m_events.push_back(std::make_unique<T>(std::forward<Args>(args)...));
    }

  private:
    void loadChunks(const Camera &camera);
    void updateChunks();
    void setSpawnPoint();

    ChunkManager m_chunkManager;

    std::vector<std::unique_ptr<IWorldEvent>> m_events;
    std::unordered_map<sf::Vector3i, ChunkSection *> m_chunkUpdates;

    std::atomic<bool> m_isRunning{true};
    std::vector<std::thread> m_chunkLoadThreads;

    // Mutex classes invoked to protect data from shared threads

    std::mutex m_mainMutex;
    std::mutex m_genMutex;

    int m_loadDistance = 2;
    const int m_renderDistance;

    glm::vec3 m_playerSpawnPoint;
};

#endif // WORLD_H_INCLUDED
