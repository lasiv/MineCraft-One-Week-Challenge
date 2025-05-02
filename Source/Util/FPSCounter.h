#ifndef FPSCOUNTER_H_INCLUDED
#define FPSCOUNTER_H_INCLUDED

#include <SFML/Graphics.hpp>

class RenderMaster;

/**
 * @class FPSCounter
 * @brief A class to count and display the frames per second (FPS) of the application.
 * 
 * @details
 * This class is used to track the number of frames rendered in a second and display
 * the FPS on the screen. It can be enabled or disabled, and it can also output the FPS
 * to the console for debugging purposes.
 *
 * The FPSCounter uses SFML's Clock to measure the time between frames and calculates
 * the FPS based on the number of frames rendered in a given time interval. The FPS
 * is displayed using an SFML Text object, which can be customized with a font and
 * character size.
*/
class FPSCounter {
  public:
    FPSCounter();

    /**
     * @brief Updates the FPS counter.
     * 
     * @details
     * This function increments the frame count and checks if the elapsed time since the last
     * update exceeds 0.5 seconds. If it does, it calculates the FPS and resets the frame count.
     * The FPS is printed to the console if debugging is enabled.
     */
    void update();

    /**
     * @brief Draws the FPS counter on the screen.
     * 
     * @param renderer The RenderMaster object used for rendering.
     * 
     * @details
     * This function sets the string of the m_text object to display the current FPS
     * and then draws it using the provided RenderMaster object.
     */
    void draw(RenderMaster &renderer);

  private:
    bool enabled = false;
    bool debugging;

    sf::Text m_text;
    sf::Font m_font;

    sf::Clock m_delayTimer;
    sf::Clock m_fpsTimer;

    float m_fps = 0;

    int m_frameCount = 0;
};

#endif // FPSCOUNTER_H_INCLUDED
