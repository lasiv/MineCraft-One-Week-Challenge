#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>

#include "../Entity.h"
#include "../Input/ToggleKey.h"
#include "../Item/ItemStack.h"
#include "../World/World.h"

#include <iostream>

class Keyboard;
class World;
class RenderMaster;

/**
 * @class Player
 * @brief Represents the player character in the game.
 *
 * @details
 * The Player class handles player input, movement, item management, and rendering.
 * It extends the Entity class to inherit position and rotation properties.
 *
 * @note This class is responsible for managing the player's state, including flying,
 * sneaking, and item selection.
 */
class Player : public Entity {
  public:
    Player();

    /**
     * @brief Handles input from the keyboard and mouse.
     * 
     * @param window The SFML window to capture input from.
     * @param keyboard The Keyboard object to manage key states.
     * 
     * @details
     * Processes keyboard and mouse input to control player actions.
     * This includes movement, item selection, and flying.
     */
    void handleInput(const sf::Window &window, Keyboard &keyboard);

    /**
     * @brief Updates the player's state based on input and world interactions.
     * 
     * @param dt The time elapsed since the last update (in seconds).
     * @param world The World object representing the game world.
     * 
     * @details
     * Updates the player's position, velocity, and interactions with the world.
     * This includes collision detection and response.
     * The player's acceleration is reset after each update.
     */
    void update(float dt, World &wolrd);

    /**
     * @brief Adds an item to the player's inventory.
     * 
     * @param material The Material object representing the item to be added.
     * 
     * @details
     * Adds a new item to the player's inventory.
     * If the item already exists, it increments the quantity.
     * If the inventory is full, it does nothing.
     */
    void addItem(const Material &material);

    /**
     * @brief Draws the player and its items to the screen.
     * 
     * @param master The RenderMaster object responsible for rendering.
     * 
     * @details
     * Renders the player model and the currently held item.
     * It also displays the player's position and item information on the screen.
     * This method is called during the rendering phase of the game loop.
     */
    void draw(RenderMaster &master);

    /**
     * @brief Returns the currently held item.
     * 
     * @return ItemStack& A reference to the currently held item.
     * 
     * @details
     * This method provides access to the item that the player is currently holding.
     * It can be used to interact with the world or perform actions.
     */
    ItemStack &getHeldItems();

    void setPosition(glm::vec3 pos);

    void setSpawn(glm::vec3 pos);

  private:

    struct LocalAABB;

    // in Player.cpp
    void printdebug(World &world) const;


    /**
     * @brief is called by update for tickbased movement
     * 
     * @param world The world object the player is in
     * 
     * @details
     * Calculates the velocity to add on the current player position for the next time tick.
     */
    void calculate(World &world);

    /**
     * @brief Handles collision detection and response with the world.
     *
     * @param world The World object representing the game world.
     * @param vel The velocity vector of the player.
     * @param dt The time elapsed since the last update (in seconds).
     *
     * @details
     * Moves the player to the calculated position, checks collision along the way and checks for edges while sneak walking.
     */
    void move(World &world);

    /**
     * @brief Handles keyboard input for player movement and actions.
     * 
     * @param keyboard The Keyboard object to manage key states.
     * 
     * @details
     * Processes input from the keyboard to control player movement.
     * This includes walking, flying, and sneaking.
     */
    void keyboardInput(Keyboard &keyboard);

    /**
     * @brief Handles mouse input for the player.
     * 
     * @param window The SFML window to capture input from.
     * 
     * @details
     * Processes mouse movement to control player view direction.
     * The mouse position is reset to the center of the window after processing.
     * The mouse input is only processed if the useMouse flag is set to true.
     */
    void mouseInput(const sf::Window &window);

    glm::vec3 m_nextPosition;

    struct LocalAABB {
        glm::vec3 min;
        glm::vec3 max;
        glm::vec3 position;
        glm::vec3 velocity;
        ChunkBlock blocks[2][4][2] = {0};
        glm::ivec3 coords[2][4][2];

        LocalAABB(const glm::vec3 position, const glm::vec3 velocity, const AABB box) {
            this->position = position;
            this->velocity = velocity;
            min = position - box.dimensions;
            max = position + box.dimensions;
        }

        void move(const glm::vec3 &delta) {
            position += delta;
            min += delta;
            max += delta;
        }

        void movex(const float delta) {
            move({delta,0.f,0.f});
        }

        void movey(const float delta) {
            move({0.f,delta,0.f});
        }

        void movez(const float delta) {
            move({0.f,0.f,delta});
        }

        void set(const glm::vec3 &location) {
            move(location - position);
        }

        void setx(const float location) {
            movex(location - position.x);
        }

        void sety(const float location) {
            movey(location - position.y);
        }

        void setz(const float location) {
            movez(location - position.z);
        }

        void setMin(const glm::vec3 &location) {
            move(location - min);
        }

        void setMax(const glm::vec3 &location) {
            move(location - max);
        }

        void setMinX(float x) { 
            movex(x - min.x); 
        }
        void setMinY(float y) { 
            movey(y - min.y); 
        }
        void setMinZ(float z) { 
            movez(z - min.z); 
        }

        void setMaxX(float x) { 
            movex(x - max.x); 
        }
        void setMaxY(float y) { 
            movey(y - max.y); 
        }
        void setMaxZ(float z) { 
            movez(z - max.z); 
        }

        void getBlocks(World &world) {
            int xs[2] = {
                int(std::floor(min.x)),
                int(std::floor(max.x))
            };

            int ys[4] = {
                int(std::floor(min.y)) - 1, // below feet
                int(std::floor(min.y)),     // feet level
                int(std::floor(position.y)),  // center to fill collision
                int(std::floor(max.y))      // head level
            };

            int zs[2] = {
                int(std::floor(min.z)),
                int(std::floor(max.z))
            };

            for (int xi = 0; xi < 2; ++xi) {
                for (int yi = 0; yi < 4; ++yi) {
                    for (int zi = 0; zi < 2; ++zi) {
                        int bx = xs[xi];
                        int by = ys[yi];
                        int bz = zs[zi];

                        blocks[xi][yi][zi] = world.getBlock(bx, by, bz);
                        coords[xi][yi][zi] = glm::ivec3(bx, by, bz);
                    }
                }
            }
        }
    
    };

    bool m_isOnGround = false;
    bool m_isInWater = false;
    bool m_isFlying = false;
    bool m_isSneaking = false;
    bool m_isSprinting = false;
    float m_slipperiness = 0.6f;
    float m_bounce = 0.0f;
    
    glm::vec3 m_spawnPoint;
    

    std::vector<ItemStack> m_items;
    std::vector<sf::Text> m_itemText;
    sf::Text m_posPrint;
    int m_heldItem = 0;

    ToggleKey m_itemDown;
    ToggleKey m_itemUp;
    ToggleKey m_flyKey;

    ToggleKey m_num1;
    ToggleKey m_num2;
    ToggleKey m_num3;
    ToggleKey m_num4;
    ToggleKey m_num5;

    struct input{
      int x;
      int y;
      int z;
    } m_input;

    inline float cube(float x) const {
      return x * x * x;
    }

    static constexpr float TICK                         = 0.05f;

    static constexpr float DEFAULT_SLIPPERINESS         = 1.0f;

    static constexpr float MOVE_MULT_SPRINT             = 1.3f;
    static constexpr float MOVE_MULT_WALK               = 1.0f;
    static constexpr float MOVE_MULT_SNEAK              = 0.3f;
    static constexpr float MOVE_MULT_STOP               = 0.0f;

    // normalization
    static constexpr float DIR_MULT_DEFAULT             = 0.98f;
    static constexpr float DIR_MULT_STRAFE_45           = 1.00f / 1.4142135623730951f;
    static constexpr float DIR_MULT_SNEAK_45            = 0.98f;

    static constexpr float BASE_FRICTION                = 0.91f; // vertical anbd flying speed reduction factor

    static constexpr float BOUNCE_TRESHOLD              = 0.05f; // speed to stop bouncing

    static constexpr float GROUND_ACCEL_BASE            = 0.1f;
    static constexpr float AIR_ACCEL_BASE               = 0.02f;
    static constexpr float WATER_ACCEL_BASE             = 0.08f;
    static constexpr float JUMP_SPRINT_BOOST            = 0.2f; 

    static constexpr float JUMP_INIT                    = 0.46f;
    static constexpr float GRAVITY_ACCEL                = 0.08f;
    static constexpr float FALLING_DRAG                 = 0.98f;

    static constexpr float WATER_FRICTION               = 0.8f; // horizontal factor
    static constexpr float WATER_DRAG                   = 0.7f; // vertical factor
    static constexpr float WATER_GRAVITY                = 0.05f;
    static constexpr float WATER_ENTRY_HEIGHT           = 0.5f; // 0.f -> entry at feet, used to disable jitter at water surface

    // Respawn height if the player falls below y ≤ 0
    static constexpr float RESPAWN_HEIGHT               = 300.0f;
};

#endif // PLAYER_H_INCLUDED
