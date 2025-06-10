#pragma once

#include <SFML/Window/Window.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include "Config.h"

/**
 * @brief Context struct that holds the window object.
 */
struct Context {

    /**
     * @brief Constructor that initializes the window based on the provided configuration.
     * 
     * @param config The configuration object containing user or system settings such as resolution, fullscreen, etc.
     * 
     * @details
     * - Creates a window with the specified settings (fullscreen or windowed).
     * - Sets OpenGL context settings (version, depth bits, etc.).
     * - Loads OpenGL functions using glad.
     * - Sets the viewport for rendering.
     * - Configures OpenGL settings (culling, blending).
     */
    Context(const Config &config);

    //sf::Window window;
    sf::RenderWindow window;
};
