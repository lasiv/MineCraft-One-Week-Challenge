#ifndef PLAYSTATE_H_INCLUDED
#define PLAYSTATE_H_INCLUDED

#include "../Player/Player.h"
#include "StateBase.h"

#include "../Input/Keyboard.h"
#include "../Util/FPSCounter.h"
#include "../World/Chunk/Chunk.h"
#include "../World/World.h"

/**
 * @class PlayState
 * @brief Manages the main gameplay loop and user interactions.
 *
 * @details
 * The PlayState class encapsulates the core gameplay mechanics, including:
 * - Handling player input and movement.
 * - Updating the game world state.
 * - Rendering the game world and player.
 * - Managing in-game events like block interactions.
 *
 * It interfaces with the World, Player, and Keyboard classes to provide a cohesive gameplay experience.
 */
class StatePlay : public StateBase {
  public:
    StatePlay(Application &app, const Config &config);

    /**
     * @brief Handles input events like key presses or window actions.
     *
     * @param e The SFML event to process.
     *
     * @details
     * Updates the internal keyboard state based on the received SFML event.
     * Called every frame by the active state handler in the application loop.
     */
    void handleEvent(sf::Event e) override;

    /**
     * @brief Processes real-time input state (keyboard/mouse).
     *
     * @details
     * Applies movement and action logic based on currently pressed keys.
     * Directly affects the player and camera movement in the world.
    */ 
    void handleInput() override;

    /**
     * @brief Updates the game logic for the current frame.
     *
     * @param deltaTime Time elapsed since the last frame (in seconds).
     *
     * @details
     * Updates the player state, world physics, and any in-game mechanics.
     * Also updates the FPS counter and other per-frame systems.
     */
    void update(float deltaTime) override;

    /**
     * @brief Renders the game world and player to the screen.
     *
     * @param renderer Reference to the RenderMaster which handles drawing.
     *
     * @details
     * Submits world chunks, player, and HUD elements to the rendering system.
     * Rendering order and visibility culling are handled here.
     */
    void render(RenderMaster &renderer) override;

    void drawUI(sf::RenderWindow &window) override;

    /**
     * @brief Called when this state is pushed onto the state stack.
     *
     * @details
     * Used to initialize or reset game-specific state when transitioning into gameplay.
     * Can be used to resume music, reset timers, or prepare HUD overlays.
     */
    void onOpen() override;

  private:
    Keyboard m_keyboard;
    Player m_player;
    World m_world;

    FPSCounter m_fpsCounter;
};

#endif // PlayState_H_INCLUDED
