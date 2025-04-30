#ifndef APPLICATION_H_INCLUDED
#define APPLICATION_H_INCLUDED

#include <memory>
#include <vector>

#include "Renderer/RenderMaster.h"
#include "States/StateBase.h"

#include "Camera.h"
#include "Context.h"

float extern g_timeElapsed;

/**
 * @class Application
 * @brief The main application class that manages the game loop and state transitions.
 *
 * @details
 * This class is responsible for initializing the application, handling events,
 * and managing different states of the application (like menus, gameplay, etc.).
 * It uses a stack to manage states, allowing for easy transitions between them.
 */
class Application {

  public:
    /**
     * @brief Constructs the Application with configuration and state initialization.
     * 
     * @param config The configuration object containing user or system settings such as resolution, fullscreen, etc.
     * 
     * @details
     * Initializes the game window using the provided configuration.
     * - Sets window resolution and properties (e.g., vsync)
     * - Initializes ImGui-SFML for GUI rendering
     * - Loads block data from the BlockDatabase (used for world/terrain logic)
     * - Pushes the initial game state onto the state manager stack
     * 
     * This prepares the core systems before entering the main run loop.
     */
    Application(const Config &config);

    /**
     * @brief The main loop of the application.
     * 
     * @details
     * - Initializes the main loop with a clock for delta time.
     * - Sets the window position to prevent it from sticking to the bottom of the screen.
     * - Enters a loop that continues until the window is closed or there are no states left.
     * - Handles input, updates the current state, and renders the frame.
     * - Calls the camera update method to adjust the view.
     * - Handles events such as window closing and key presses.
     * - If a state is popped, it removes the last state from the stack.
     * - Updates the elapsed time for the application.
     */
    void runLoop();

    /**
     * @brief Pushes a new state onto the state stack.
     * 
     * @tparam T The type of the state to be pushed.
     * @tparam Args The types of the arguments to be passed to the state constructor.
     * @param args The arguments to be passed to the state constructor.
     * 
     * @details
     * - Creates a new instance of the specified state type and pushes it onto the stack.
     * - Calls the onOpen method of the new state to initialize it.
     */
    template <typename T, typename... Args> void pushState(Args &&... args)
    {
        m_states.push_back(std::make_unique<T>(std::forward<Args>(args)...));
        auto &s = m_states.back();
        s->onOpen();
    }

    /**
     * @brief Tell the program stack to pop off the state.
     */
    void popState();

    /**
     * @brief Returns camera object.
     */
    Camera &getCamera()
    {
        return m_camera;
    }

    /**
     * @brief Returns the context window object.
     */
    const sf::Window &getWindow() const
    {
        return m_context.window;
    }

    /**
     * @brief Makes the mouse invisible, doesn't actually turn off the mouse. 
     */ 
    void turnOffMouse();

    /**
     * @brief Makes the mouse visible again. 
     */ 
    void turnOnMouse();

  private:
    /**
     * @brief Handles events such as window closing and key presses.
     * 
     * @details
     * - Polls for events from the window.
     * - Calls the handleEvent method of the current state.
     * - Handles specific events like window closing and key presses (e.g., Escape).
     * - Closes the window if the Escape key is pressed.
     * - Sets the mouse cursor visibility based on the state.
     */
    void handleEvents();

    /**
     * @brief state stack for managing different states of the application.
     */
    std::vector<std::unique_ptr<StateBase>> m_states;
    
    Context m_context;
    RenderMaster m_masterRenderer;
    Camera m_camera;

    const Config &m_config;

    bool m_isPopState = false;
};

#endif // APPLICATION_H_INCLUDED
