#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>

#include "../Entity.h"
#include "../Input/ToggleKey.h"
#include "../Item/ItemStack.h"

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
     * @brief Handles collision detection and response with the world.
     * 
     * @param world The World object representing the game world.
     * @param vel The velocity vector of the player.
     * @param dt The time elapsed since the last update (in seconds).
     * 
     * @details
     * Checks for collisions with blocks in the world and adjusts the player's position accordingly.
     * This method is called during the update phase to ensure the player does not pass through blocks.
     * It also handles the player's flying state.
     */
    void collide(World &world, const glm::vec3 &vel, float dt);

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

  private:
    void jump();

    void keyboardInput(Keyboard &keyboard);
    void mouseInput(const sf::Window &window);
    bool m_isOnGround = false;
    bool m_isFlying = false;
    bool m_isSneak = false;

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

    ToggleKey m_slow;

    glm::vec3 m_acceleration;
};

#endif // PLAYER_H_INCLUDED
