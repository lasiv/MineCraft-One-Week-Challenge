#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

/**
 * @struct Config
 * @brief Configuration structure for application settings.
 * 
 * @details
 * This structure holds various settings for the application, including:
 * - Window dimensions (width and height)
 * - Fullscreen mode (true/false)
 * - Render distance (how far the game world is rendered)
 * - Field of view (FOV) for the camera
 * 
 * @note
 * The default values are set to reasonable defaults for a typical gaming experience.
 * These can be modified by the user through a configuration file.
 */
struct Config {
    int windowX = 1280;
    int windowY = 720;
    bool isFullscreen = false;
    int renderDistance = 8; // Set initial RD low to prevent long load times
    int fov = 90;
};

#endif // CONFIG_H_INCLUDED
