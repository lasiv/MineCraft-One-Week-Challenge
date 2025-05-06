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
     * @brief Updates the chunk at the specified coordinates.
     * 
     * @param blockX The x-coordinate of the block.
     * @param blockY The y-coordinate of the block.
     * @param blockZ The z-coordinate of the block.
     * 
     * @details
     * This function updates the chunk at the specified world coordinates. It calculates
     * the chunk position and block position within the chunk, and updates the corresponding
     * chunk section. The coordinates are expected to be in world space.
     */
    void updateChunk(int blockX, int blockY, int blockZ);

    /**
     * @brief Renders the world using the specified RenderMaster and camera.
     * 
     * @param master The RenderMaster object used for rendering.
     * @param camera The camera object used for rendering the world.
     * 
     * @details
     * This function renders the world using the specified RenderMaster and camera. It
     * handles the rendering of chunks, sky, and other world elements. The camera is used
     * to determine the view frustum and culling of chunks.
     */
    void renderWorld(RenderMaster &master, const Camera &camera);

    /**
     * @brief Gets the chunk manager for the world.
     * 
     * @return A reference to the ChunkManager object.
     * 
     * @details
     * This function returns a reference to the ChunkManager object, which is responsible
     * for managing the chunks in the world. The ChunkManager handles loading, unloading,
     * and generating chunks, as well as managing the terrain generator.
     */
    ChunkManager &getChunkManager();

    /**
     * @brief Gets the player spawn point.
     * 
     * @return The player spawn point as a glm::vec3.
     * 
     * @details
     * This function returns the player spawn point as a glm::vec3. The spawn point is
     * randomly generated within the world bounds and is used to spawn the player when
     * entering the world.
     */
    static VectorXZ getBlockXZ(int x, int z);
    /**
     * @brief Gets the chunk coordinates for the specified world coordinates.
     * 
     * @param x The x-coordinate of the block.
     * @param z The z-coordinate of the block.
     * 
     * @return The chunk coordinates as a VectorXZ object.
     * 
     * @details
     * This function calculates the chunk coordinates for the specified world coordinates.
     * It returns a VectorXZ object containing the chunk coordinates based on the world
     * coordinates provided.
     */
    static VectorXZ getChunkXZ(int x, int z);

    // void collisionTest(Entity &entity);

    template <typename T, typename... Args> void addEvent(Args &&... args)
    {
        m_events.push_back(std::make_unique<T>(std::forward<Args>(args)...));
    }

  private:
    /**
     * @brief Loads chunks around the player based on the camera position.
     * 
     * @param camera The camera object used to determine the current view.
     * 
     * @details
     * This function loads chunks around the player based on the camera position. It
     * calculates the chunk coordinates based on the camera position and loads the
     * corresponding chunks. The load distance determines how many chunks are loaded
     * around the player.
     */
    void loadChunks(const Camera &camera);

    /**
     * @brief Updates the chunks based on the current state of the world.
     * 
     * @details
     * This function updates the chunks based on the current state of the world. It
     * processes any pending chunk updates and applies them to the corresponding chunks.
     * It also handles the generation of new chunks if necessary.
     */
    void updateChunks();

    /**
     * @brief Sets the spawn point for the player in the world.
     * 
     * @details
     * This function sets the spawn point for the player in the world. It generates a
     * random spawn point within the world bounds and sets it as the player's spawn
     * point. The spawn point is used to spawn the player when entering the world.
     * It ensures that the spawn point is valid and does not collide with any existing
     * blocks or terrain features.
     */
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
