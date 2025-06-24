#ifndef STATEBASE_H_INCLUDED
#define STATEBASE_H_INCLUDED

#include <SFML/Graphics.hpp>

class RenderMaster;
class Application;

/**
 * @class StateBase
 * @brief Abstract base class for all game states.
 *
 * @details
 * Defines the required interface for all states in the game (e.g., PlayState, MenuState).
 * Each state must implement its own versions of event handling, input processing, update,
 * rendering, and activation logic.
 * 
 * This class is used by the state stack system within the Application class to allow
 * dynamic switching and stacking of different game states (such as gameplay, menus, or pause).
 * 
 * @note All member functions are pure virtual, making this a strict interface class.
 */
class StateBase {
  public:
    StateBase(Application &app)
        : m_pApplication(&app)
    {
    }

    virtual ~StateBase() = default;

    /**
     * @brief Handles window or input events (e.g., keyboard, mouse).
     *
     * @param e The SFML event to be processed.
     *
     * @details
     * Called once per frame when events are polled from the window.
     * Used to update internal input state or respond to window actions.
     */
    virtual void handleEvent(sf::Event e) = 0;

    /**
     * @brief Processes continuous input (e.g., held keys).
     *
     * @details
     * Called every frame, separate from SFML event handling.
     * Use this to move players, interact with the world, etc., based on current input state.
     */
    virtual void handleInput() = 0;

    /**
     * @brief Updates the logic of the current state.
     *
     * @param deltaTime Time in seconds since the last frame.
     *
     * @details
     * Called once per frame to update world, entities, animations, timers, etc.
     */
    virtual void update(float deltaTime) = 0;

    /**
     * @brief Renders the current state to the screen.
     *
     * @param renderer The RenderMaster object that handles rendering operations.
     *
     * @details
     * Submit draw calls here (world, UI, entities). Called after update each frame.
     */
    virtual void render(RenderMaster &renderer) = 0;

    virtual void drawUI(sf::RenderWindow &window) = 0;

    /**
     * @brief Called when this state becomes the active state.
     *
     * @details
     * Use this to initialize or reset state-specific systems, load resources,
     * or resume paused activity.
     */
    virtual void onOpen() = 0;

  protected:
    Application *m_pApplication;
};

#endif // STATEBASE_H_INCLUDED
